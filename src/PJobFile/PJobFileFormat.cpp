#include "PJobFileFormat.h"
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QVariant>
#include <QtCore/QDateTime>

PJobFileFormat::PJobFileFormat(QString path):QFile(path)
//lädt eine .pjob-Datei oder erstellt eine leere .pjob-Datei (inklusive Header) falls diese noch nicht existiert
{
    m_path=path;
    m_modified=false;
    m_version = c_version;

    //Wenn relativer Pfad angegeben wurde, zunächst in absoluten Pfad umrechnen
    if(QDir::isRelativePath(m_path))
    {
        //aktuellen Verzeichnispfad auslesen
        QString absPath = QDir::currentPath();

        //absoluten Pfad anpassen
        m_path = absPath + '/' + m_path;
    }

    //Verzeichnis erstellen, falls dieses nicht exisitert
    QDir dir(m_path.section('/',0,-2));
    if(!dir.exists())
        dir.mkpath(m_path.section('/',0,-2));

    //Datei öffnen
    if(!QFile::open(QIODevice::ReadWrite))
        throw ReadFileError(QString("Could not open file %1!").arg(path));

	//Inhalt der Datei zur Bearbeitung in m_data kopieren
	this->seek(0);
	m_data.append(this->readAll());

    //Header schreiben falls Datei nicht existiert
//~~isValid() wird NUR EINMAL aufgerufen, wenn Datei geöffnet wird
//~~Wenn die Datei nicht valid ist wird sie (z.Z.) überschrieben! (leere Datei)
    if(!this->isValid())
	{
        createNewFile();
	}
    else
        this->map();
}

PJobFileFormat::~PJobFileFormat()
{
    this->close();
}

bool PJobFileFormat::appendFile(QString sourceAbsolutePath, QString targetRelativePath, bool overwrite)
{
    while(targetRelativePath.startsWith("/") || targetRelativePath.startsWith("\\"))
        targetRelativePath.remove(0,1);

    m_modified=true;
	
    //Wenn kein Zielpfad spezifiziert wurde, wird nur der Dateiname gespeichert (Rootverzeichnis)
    if(targetRelativePath==NULL)
        targetRelativePath=sourceAbsolutePath.section('/',-1);

    //Wenn relativer Pfad angegeben wurde, zunächst in absoluten Pfad umrechnen
    if(QDir::isRelativePath(sourceAbsolutePath))
    {
        sourceAbsolutePath = m_path.section('/',0,-2) + '/' + sourceAbsolutePath;
    }

    //Datei öffnen
    QFile fileToAdd(sourceAbsolutePath);
    if(!fileToAdd.open(QIODevice::ReadOnly))
		throw ReadFileError("Couldn't add \"" + sourceAbsolutePath + "\" to .pjob-file.");

	QByteArray data = fileToAdd.readAll();
    quint64 mtime = QFileInfo(fileToAdd).lastModified().toTime_t();
    fileToAdd.close();

	//Datei hinzufügen
	if(appendFile(data, targetRelativePath, mtime, overwrite) == false){
		return false;
	}

	emit output(targetRelativePath);

	return true;
}

bool PJobFileFormat::appendFile(const QByteArray &source, QString targetRelativePath, quint64 mtime, bool overwrite)
{
    m_modified=true;
    while(targetRelativePath.startsWith("/") || targetRelativePath.startsWith("\\"))
        targetRelativePath.remove(0,1);

	// Wenn kein Änderungsdatum übergeben wurde, setze mtime auf momentanen Zeitpunkt
	if(mtime == 0) mtime = QDateTime::currentDateTime().toTime_t();

	//Überprüfen, ob Zieldatei nicht schon exisitiert
	if(this->contains(targetRelativePath))
	{
		if(overwrite)
			this->removeFile(targetRelativePath); 
		else
		{
			emit output("File already exists: " + targetRelativePath + "  use --force to overwrite");
			return false;
		}
	}

	//Auf Kollisionen mit Windows-Dateisystem prüfen
    if(!this->proofUnique(targetRelativePath))
		throw FileSystemError("Tried to add \"" + targetRelativePath + "\" to .pjob-file, but a filesystem collision was detected. Please choose another name for the file. Aborting...");

    //Komprimieren und 4 Bytes am Anfang des resultierenden Arrays "wegschmeißen" (Kompatibilität zu zlib)
    QByteArray compressed = qCompress(source,9).remove(0,4);

    //Map aktualisieren
    this->addToMap(targetRelativePath, m_data.size(), compressed.size());

    //Header schreiben:
    //Dateiname ('\n'-terminierter String)
    m_data.append(targetRelativePath+'\n');

    //Änderungsdatum schreiben in Sekunden seit 01.01.1970 (8-Byte-Integer)
    writeInt64(mtime,m_data,m_data.size());

    //Dateigröße schreiben in Byte (4-Byte-Integer)
    writeInt32(source.size(),m_data,m_data.size());

    //Gepackte Dateigröße schreiben in Byte (4-Byte-Integer)
    writeInt32(compressed.size(),m_data,m_data.size());

    //Datei schreiben
    m_data.append(compressed);

	//Information über hinzugefügte Datei versenden
    QList<QVariant> fileProperties;
    fileProperties << QVariant(targetRelativePath) << QVariant(mtime) << QVariant(source.size()) << QVariant(compressed.size());
    emit fileAdded(fileProperties);

    return true;
}

bool PJobFileFormat::appendRaw(const QByteArray &source)
{
	//Daten auslesen
	int size = m_data.size();
	QString filename = source.left(source.indexOf('\n'));

	//Überprüfen, ob Zieldatei nicht schon exisitiert
	if(this->contains(filename))
		return false;

	//Auf Kollisionen mit Windows-Dateisystem prüfen
	if(!this->proofUnique(filename))
		return false;

	//Datei 'testweise' hinzufügen und überprüfen ob .pjob-Datei noch valid ist
	m_data.append(source);

	if(!this->isValid())
	{
		//Änderungen wieder verwerfen und Abbruch
		m_data.truncate(size);
		throw RawDataError("Appending raw data failed because the data was not valid.");
	}

	//Map aktualisieren
	this->addToMap( filename , size , source.size() );
	m_modified = true;

	return true;
}

bool PJobFileFormat::appendFolder(QString sourceAbsolutePath, QString targetRelativePath, bool overwrite)
{
    m_modified = true;

    while(targetRelativePath.startsWith("/") || targetRelativePath.startsWith("\\"))
        targetRelativePath.remove(0,1);

    QDir current = sourceAbsolutePath;
    QFileInfo entry(sourceAbsolutePath);

    //AppendFolder soll auch funktionieren, falls man nur eine einzelne Datei auswählt (z.B. via Kommandozeile)
    if(entry.isFile()&&(!(entry.isDir())))
    {
        if(!(this->appendFile(entry.absoluteFilePath(), targetRelativePath, overwrite )))
            return false;
        //Anschließend ist keine Rekursion mehr erforderlich
        return true;
    }

    // "/" an relativen Pfad anhängen falls benötigt
    if(!targetRelativePath.endsWith("/") && targetRelativePath.size() > 0) targetRelativePath.append("/");

    //Über alle Dateien iterieren und Hinzufügen
    foreach(entry, current.entryInfoList(QDir::Files))
    {
        this->appendFile(entry.absoluteFilePath(), targetRelativePath + entry.fileName() , overwrite );
    }

    //Über alle Ordner iterieren und rekursiv hinzufügen
    foreach(QString s, current.entryList(QDir::Dirs))
    {
		QMapIterator<QString, intPair> iterator(m_map);

		//Sobald eine Datei gefunden wird, welche identisch ist mit dem Beginn des hinzuzufügenden Ordners, darf dieser nicht hinzugefügt werden
		while (iterator.hasNext())
		{
			iterator.next();
			if((targetRelativePath + s).startsWith(iterator.key()+ '/'))
			{
				emit output("Detected a filesystem collision. Aborting...");
				return false;
			}
		}

        //Alle Oberverzeichnisse werden nicht hinzugefügt
        if(! (sourceAbsolutePath.endsWith("/") || sourceAbsolutePath.endsWith("\\")))
            sourceAbsolutePath.append("/");
        if(! (targetRelativePath.endsWith("/") || targetRelativePath.endsWith("\\")))
            targetRelativePath.append("/");
        if((s!=".")&&(s!=".."))
            if(!(this->appendFolder(sourceAbsolutePath + s + '/', targetRelativePath + s +'/' , overwrite )))
                return false;        
    }
    return true;
}

bool PJobFileFormat::rename(QString oldPath, QString newPath){
	// Überprüfe ob eine Datei übergeben wurde
	if(!oldPath.endsWith("/")){
		if(this->contains(oldPath)){
			m_data.replace(m_map[oldPath].position, oldPath.length(), QByteArray().append(newPath));
			this->map();
			m_modified = true;
			return true;
		}
	}

	// Wenn keine Datei gefunden wurde, nehme an es handelt sich um einen Ordner
	if(!oldPath.endsWith("/")) oldPath = oldPath + "/";
	if(!newPath.endsWith("/")) newPath = newPath + "/";
	QMapIterator<QString, intPair> iterator(m_map);

	// Durch die Änderung von Dateinamen entstehen Abweichungen von den in der Map
	// angegebenen Datei Positionen. Diese werden durch die posError Map ausgeglichen,
	// deren Keys der Position einer Datei in der PJob Datei entsprechen. Das value
	// entspricht der Abweichung, die ab einer Position/ einem Key wirkt.
	// Beispiel:
	// (--posError.lowerBound(700)) gibt das vor dem Key 700 gelegene Key/Value
	// Paar zurück, dessen Value der Abweichung zur Ursprünglichen Position 700 entspricht.
	int oldPathLength = oldPath.length();
	int pathDiff = newPath.length() - oldPathLength;
	QMap<int, int> posError;
	posError.insert(0,0);
	bool entriesChanged = false;

	// Alle Pfade im QByteArray updaten
	int filePosition, localPosError;
	while(iterator.hasNext()){
		iterator.next();
		if(iterator.key().startsWith(oldPath)){
			filePosition = iterator.value().position;
			localPosError = (--posError.lowerBound(filePosition)).value();
			m_data.replace(filePosition + localPosError, oldPathLength, QByteArray().append(newPath));
			posError.insert(filePosition, localPosError + pathDiff);
			entriesChanged = true;
		}
	}
	// Wenn keine Dateien umbenannt wurden, gib false zurück
	if(entriesChanged == false) return false;

	// Und jetzt die Map aktualisieren
	this->map();

	m_modified = true;
	return true;
}

bool PJobFileFormat::contains(QString relativePath)
{
    //Map durchsuchen und entsprechenden Wert zurückgeben
    return m_map.contains(relativePath);
}

bool PJobFileFormat::containsDirectory(QString relativePath)
{
    QMapIterator<QString, intPair> iterator(m_map);

    //Sobald ein Verzeichnis gefunden wird, welches mit relativePath beginnt, wird true zurückgegeben
	//Es wird nicht sichergestellt, dass es sich auch tatsächlich um ein Verzeichnis handelt
    while (iterator.hasNext())
    {
        iterator.next();
        if(iterator.key().indexOf(relativePath)==0)
            return true;
    }
    return false;
}

QByteArray PJobFileFormat::readFile(QString relativePath)
{
    //Leeres QByteArray zurückgeben, falls Datei nicht existiert ~~throw something?
    if(!this->contains(relativePath))
        return NULL;

    //Position und Größe der Datei holen
    intPair info = m_map.find(relativePath).value();
	QString path = m_map.find(relativePath).key();

    //Auslesen
    QByteArray toRead = m_data.mid(info.position + path.size() + c_fileHeaderSize,info.size);
	int len = info.size;

    //4 Byte vorne anhängen und Dekomprimieren
    for(int i=0;i<4;i++)
    {
        toRead.prepend(len);
		len/=256;
    }
	return qUncompress(toRead);
}

QByteArray PJobFileFormat::readRaw(QString relativePath)
{
	//Wenn die Datei nicht existiert wird nichts zurückgegeben
	if(!this->contains(relativePath))
		return NULL;

	//Position und Größe der Datei holen
	intPair info = m_map.find(relativePath).value();
	QString path = m_map.find(relativePath).key();

	//entsprechendes QByteArray zurückgeben
	return m_data.mid(info.position, path.size()+info.size+c_fileHeaderSize);
}

bool PJobFileFormat::removeFile(QString relativePath)
{
    //Wenn keine Datei zu löschen ist wird false zurückgeben
    if(!this->contains(relativePath))
        return false;

    //Position und Größe der Datei holen
    intPair info = m_map.find(relativePath).value();
    QString path = m_map.find(relativePath).key();

    //Datei entfernen
    m_data.remove(info.position, path.size()+info.size+c_fileHeaderSize);

    //Die Positionen aller Dateien werden aktualisiert
	this->map();
	
	m_modified=1;
	return true;
}

bool PJobFileFormat::remove(QString relativePath){
	// Überprüfe ob eine Datei übergeben wurde
	if(!relativePath.endsWith("/")){
		if(removeFile(relativePath) == true) return true;
	}

	// Wenn keine Datei gefunden wurde, nehme an es handelt sich um einen Ordner
	if(!relativePath.endsWith("/")) relativePath = relativePath + "/";
	QMapIterator<QString, intPair> iterator(m_map);

	// Durch das Löschen von Dateien entstehen Abweichungen von den in der Map
	// angegebenen Datei Positionen. Diese werden durch die posError Map ausgeglichen,
	// deren Keys der Position einer Datei in der PJob Datei entsprechen. Das value
	// entspricht der Abweichung, die ab einer Position/ einem Key wirkt.
	// Beispiel:
	// (--posError.lowerBound(700)) gibt das vor dem Key 700 gelegene Key/Value
	// Paar zurück, dessen Value der Abweichung zur Ursprünglichen Position 700 entspricht.
	QMap<int, int> posError;
	posError.insert(0,0);
	bool entriesChanged = false;

	// Alle Dateien im Pfad löschen
	int filePosition, fileEntrySize, localPosError;
	while(iterator.hasNext()){
		iterator.next();
		if(iterator.key().startsWith(relativePath)){
			filePosition = iterator.value().position;
			fileEntrySize = iterator.key().length() + iterator.value().size + c_fileHeaderSize;
			localPosError = (--posError.lowerBound(filePosition)).value();
			m_data.remove(filePosition - localPosError, fileEntrySize);
			posError.insert(filePosition, localPosError + fileEntrySize);
			entriesChanged = true;
		}
	}
	// Wenn keine Dateien gelöscht wurden, gib false zurück
	if(entriesChanged == false) return false;

	// Und jetzt die Map aktualisieren
	this->map();

	m_modified = true;
	return true;
}

void PJobFileFormat::extract(QString targetAbsolutePath, QString sourceRelativePath, bool overwrite)
{
    QMapIterator<QString, intPair> iterator(m_map);
    QDir targetDirectory;
    QString currentAbsolutePath;

    //Rootverzeichnis (Pfad in dem die .pjob-Datei gespeichert ist), wenn kein Zielpfad angegeben
    if(targetAbsolutePath==NULL)
        targetAbsolutePath = m_path.section('/',0,-2)+'/';
	
	//sicherstellen, dass Ordner-Pfad mit '/' endet
	if(!targetAbsolutePath.endsWith('/'))
		targetAbsolutePath += '/';

    //Wenn relativer Pfad angegeben wurde, zunächst in absoluten Pfad umrechnen
    if(QDir::isRelativePath(targetAbsolutePath))
        targetAbsolutePath = m_path.section('/',0,-2) + '/' + targetAbsolutePath;

    //Map durchlaufen und jeweilige Dateien entpacken
    while(iterator.hasNext())
    {
        iterator.next();
		
        //Wenn aktuelle Datei mit dem zu entpackenden Pfad beginnt oder alle Dateien entpackt werden sollen
        if(iterator.key().indexOf(sourceRelativePath)==0 || sourceRelativePath==0)
        {
          //ggf. Verzeichnis erstellen
			
			/*********
			 *verhindern, dass das Quell-Verzeichnis/die Quell-datei im Parent-folder entpackt wird
			 *indem sourceRelativePath (ohne den letzten Ordner) vom Beginn weggeschnitten wird*/
			
			/*
			Dieser auskommentierte Code führt stattdessen dazu, dass die innere Ordnerstruktur erhalten bleibt (vielleicht wird das doch nochmal gebraucht)
			currentAbsolutePath =  targetAbsolutePath + iterator.key().section('/',0,-2) +'/';
			*/

			QString currentFilePath = iterator.key();
			int len = 0;
			if(sourceRelativePath != 0)
			{
				if(sourceRelativePath.endsWith('/'))
					len = sourceRelativePath.section('/',0,-3).length();
				else
					len = sourceRelativePath.section('/',0,-2).length();
			}
			currentFilePath = currentFilePath.right(currentFilePath.length()-len);
            currentAbsolutePath =  targetAbsolutePath + currentFilePath.section('/',0,-2);
			//*********/
            
			targetDirectory.setPath(currentAbsolutePath);
            if(!targetDirectory.exists())
                targetDirectory.mkpath(currentAbsolutePath);

          //Datei schreiben
            //Datei wird in das targetAbsolutePath-Verzeichnis (+ relativer Pfad) (über)schrieben
            //Dateipfad lesen und öffnen
            QFile file(targetAbsolutePath + currentFilePath);
            if((!file.exists())||overwrite)
            {
                if(!file.open(QIODevice::WriteOnly))
					throw ReadFileError("Could not extract file \"" + targetAbsolutePath + iterator.key() + "\". There is not enough disk space or the file is write-protected.");
               
				//Auslesen
                int pos = iterator.value().position + iterator.key().size() + c_fileHeaderSize;
                int len = iterator.value().size;
                QByteArray toWrite(m_data.mid(pos, len));

                //~~ 4 Byte als BigEndian vorne anhängen und Dekomprimieren
                for(int i=0;i<4;i++)
                {
                    toWrite.prepend(len);
                    len/=256;
                }
                toWrite=qUncompress(toWrite);

                //Datei schreiben
                file.write(toWrite);
                file.resize(toWrite.size());
                //TODO: Datei Änderungsdatum zurückschreiben(boost::filesystem::last_write_time( )?)
                file.flush();
                file.close();
			    emit output(targetAbsolutePath + iterator.key());
            }
			else
				emit output("File already exists: " + targetAbsolutePath + iterator.key() + "  use --force to overwrite");
        }
    }
}

QStringList PJobFileFormat::content()
{
    //Hilfsvariablen
    QStringList list;
    QMapIterator<QString, intPair> iterator(m_map);

    //Durchlaufen der Map und Dateinamen in Liste schreiben
    while (iterator.hasNext())
    {
         iterator.next();
         list.append(iterator.key());
    }

    //Liste sortieren und ausgeben
    list.sort();
    return list;
}

QList<QList<QVariant> > PJobFileFormat::detailedContents(QString folder)
{
        QList<QList<QVariant> > contents;
	QMapIterator<QString, intPair> iterator(m_map);

	//Durchlaufen der Map
	while (iterator.hasNext())
	{
		iterator.next();
		if(!iterator.key().startsWith(folder + "/")) continue;
		QList<QVariant> list = QList<QVariant>();

		// Schreibe Dateinamen in die Liste
		list << QVariant(QString(iterator.key()));

		// Schreibe Datum in die Liste
		list << QVariant( readInt64(m_data, iterator.value().position + iterator.key().length() + 1) );

		// Schreibe Dateigröße in die Liste
		list << QVariant( readInt32(m_data, iterator.value().position + iterator.key().length() + 1 + 8));

		// Schreibe gepackte Dateigröße in die Liste
		list << QVariant(iterator.value().size);

		contents << list;
	}
	return contents;
}

int PJobFileFormat::version()
{
	return c_version;
}

//AB HIER PRIVATE

bool PJobFileFormat::isValid()
{
	int n = m_data.size();

    //Header überprüfen
	if((n!=0)&&(n<21))
		return false;

	if(!m_data.startsWith("PJobFile\n"))
        return false;
	
	//nicht unterstützte Version
	if(!(readInt32(m_data,9)<=m_version))
		return false;

	//Hier geht das iterative Parsen los
	int pos = 21;
	while(pos != n)
	{
		//Dateinamen überspringen
		int pos2 = m_data.indexOf('\n',pos);

		//Fehler im Dateiheader
		if((pos2 == -1)||(pos2+c_fileHeaderSize > n)||(pos2==pos))
			return false;
		
		//Dateigröße parsen
		int size = readInt32(m_data,pos2+c_fileHeaderSize-4);

		//ans Ende der aktuellen Datei springen
		pos = pos2 + c_fileHeaderSize + size;

		//überprüfen ob pos noch innerhalb der Arraygrenzen liegt
		if(pos > n)
			return false;
	}
    return true;
}

void PJobFileFormat::createNewFile()
{
    m_modified=true;

	//alten Inhalt löschen
	m_data.truncate(0);

	//Header schreiben
    this->writeHeaderInformation(false);
}

void PJobFileFormat::writeHeaderInformation(bool overwriteOldHeader)
{
    int length=0;
    if(overwriteOldHeader)
        length=21;

    //temporäres Array mit Headerinhalt schreiben
    QByteArray temp("PJobFile\n");
    writeInt32(m_version,temp,9);
	writeInt64(QDateTime::currentDateTime().toTime_t(),temp,13);

    //Headerbereich in m_data schreiben/ersetzen
    m_data.replace(0,length,temp);
}

void PJobFileFormat::map()
{
	//Map zurücksetzen
	QMap<QString,intPair> map;
	m_map = map;

    QString filePath;
    quint32 headerSize, dataSize;
    int pos=21;
    intPair info;

    //Solange noch nicht alle Dateien in die Map aufgenommen wurden
    while(pos < m_data.size())
    {
      //Variablen setzen
        //Headergröße auslesen
        headerSize=m_data.indexOf('\n',pos)-pos+c_fileHeaderSize;

        //Dateigröße auslesen
        dataSize=readInt32(m_data,pos+headerSize-4);

        //Dateinamen auslesen
        filePath = m_data.mid(pos,headerSize-c_fileHeaderSize);

        //Datei in die Map aufnehmen
        info.position = pos;
        info.size = dataSize;
        m_map.insert(filePath,info);

        //Position ändern
        pos += headerSize + dataSize;
    }
}

void PJobFileFormat::addToMap(QString relativePath, int position, int size)
{
    intPair info;

    //Werte für Integerpaar setzen
    info.position=position;
    info.size=size;

    //Wert der Map hinzufügen
    m_map.insert(relativePath,info);
}

QString PJobFileFormat::adjustPath(QString path)
{
    QFileInfo info(path);
    if(info.isDir())
        return info.absolutePath() + '/';
    else
        return info.absoluteFilePath();
}

bool PJobFileFormat::proofUnique(QString relativePath)
{
	//Wenn die .pjob-Datei bereits einen Ordner mit gleichem Namen enthält...
	if(this->containsDirectory(relativePath))
	{
		emit output("Detected a filesystem collision. Aborting...");
		return false;
	}

	//... oder eine Datei mit einem Teilpfad der hinzuzufügenden Datei beginnt
	QMapIterator<QString, intPair> iterator(m_map);
	while (iterator.hasNext())
	{
		iterator.next();
		if(relativePath.startsWith(iterator.key()+ '/'))
		{
			emit output("Detected a filesystem collision. Aborting...");
			return false;
		}
	}
	//sonst ist alles super
	return true;
}

void PJobFileFormat::writeInt32(quint32 input, QByteArray &array, int pos)
{
  //Byteweises Auslesen von Input als char*
    char* a = (char*)&input;
//~~Maschinenabhängig! I.d.R. LittleEndian, für BigEndian: for(int i=3;i>=0;i--) ggf. mit Qt-Klasse abfangen?
    for(int i=0;i<4;i++)
        array.insert(pos++,a[i]);
}

void PJobFileFormat::writeInt64(quint64 input, QByteArray &array, int pos)
{
  //Byteweises Auslesen von Input als char*
    char* a = (char*)&input;
//~~Maschinenabhängig! I.d.R. LittleEndian, für BigEndian: for(int i=7;i>=0;i--) ggf. mit Qt-Klasse abfangen?
    for(int i=0;i<8;i++)
        array.insert(pos++,a[i]);
}

quint32 PJobFileFormat::readInt32(const QByteArray &array, int pos)
{
//~~Byteweises Auslesen der nächsten 4 Bytes beginnend ab pos als 32-Bit-Integer ~~ falls BigEndian: Probleme
    quint32 j=0;
    for(int i=0;i<4;i++)
        j+=static_cast<unsigned char>(array.at(pos++))*power(256,i);
    return j;
}

quint64 PJobFileFormat::readInt64(const QByteArray &array, int pos)
{
//~~Byteweisen Auslesen der nächsten 8 Bytes beginnend ab pos als 64-Bit-Integer ~~ falls BigEndian: Probleme
    quint64 j=0;
    for(int i=0;i<8;i++)
        j+=static_cast<unsigned char>(array.at(pos++))*power(256,i);
    return j;
}

void PJobFileFormat::flush()
{
    //Wenn File unverändert muss nichts gespeichert werden
    if(!m_modified)
        return;
    m_modified=false;

    //Header Aktualisieren
    writeHeaderInformation(true);

    //.pjob-Datei mit m_data synchronisieren
    this->seek(0);
    this->write(m_data);
    this->resize(m_data.size());
    if(!QFile::flush())
		throw WriteFileError("Could save changes to file \"" + m_path + "\". Maybe the file is write-protected?");
}

quint32 PJobFileFormat::power(const int base, int exponent)
{
    int result=1;
    while((exponent--)>0)
        result*=base;
    return result;
}
