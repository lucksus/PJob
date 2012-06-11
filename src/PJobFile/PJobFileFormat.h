#pragma once
#include "PJobFileError.h"
#include <QtCore/QFile>
#include <QtCore/QByteArray>
#include <QtCore/QStringList>
#include <QtCore/QMap>
#include <QtCore/QVariant>

//! Kapselung des Zugriffs auf unser PJob-Dateiformat
/*!
  * @author Daniel Merget
  * @date 26.02.2010
  *
  * Bei der Übergabe von Pfaden folgendes beachten:
  *  - Strings sind grundsätzlich Case-Sensitive
  *  - Pfade verwenden grundsätzlich '/' als Trennzeichen
  *  - Pfade zu Ordnern enden immer mit '/'
  *  - Pfade beginnen niemals mit '/'
  */

class PJobFileFormat : public QFile
{
Q_OBJECT
public:

	//! Version des PJob Dateiformats. Muss erhöht werden, wenn keine abwärts Kompatibilität mehr gegeben ist.
	static const int c_version=2;
	//! Größe des Headers jeder Datei in einer PJob Datei ohne Dateiname.
	/*!
	 * Struktur: 1 Byte \n am Ende jedes Dateinamens, 8 Byte Modification Time, 4 Byte Dateigröße, 4 Byte gepackte Dateigröße
	 * ACHTUNG: Bei Änderungen der Struktur müssen alle Funktionen auf Korrektheit überprüft werden und gegebenenfalls angepasst werden.
	 * Dies ist trotz der Variable c_fileHeaderSize nötig, wenn Funktionen am Dateiheader(Auslesen/ Schreiben einzelner Attribute) arbeiten.
	 */
	static const int c_fileHeaderSize = 17;

        //! Öffnet eine .pjob-Datei. Falls diese nicht exisitert wird eine leere Datei (inklusive Header) erzeugt.
        /*!
         *  @param path kann relativ zum Arbeitsverzeichnis oder absolut angegeben werden.
         */
        PJobFileFormat(QString path);

        //! Erstellt ein Objekt mit den bereits in das übergebene QByteArray geladenen Daten.
        /*!
          * @param data Inhalt einer PJob-Datei
          */
        PJobFileFormat(const QByteArray& data);

        //! Destruktor verwirft alle veränderten Daten, wenn nicht voher flush() aufgerufen wurde.
        ~PJobFileFormat();

        //SCHREIBEN
        //! Fügt eine beliebige Datei sourceAbsolutePath (oder ein QByteArray) unter dem Ordner-/Dateipfad targetRelativePath hinzu. Gibt true zurück, falls erfolgreich. Wenn die Datei unter diesem Namen bereits existiert wird sie überschrieben!
        /*!
         *  @param sourceAbsolutePath ist der Pfad der hinzuzufügenden Datei. Darf auch relativ zum Pfad der .pjob-Datei angegeben werden.
         *  @param targetRelativePath gibt den Pfad an, welcher innerhalb der .pjob-Datei für die Datei reserviert werden soll. Falls NULL übergeben wird, wird nur der Name der Datei ohne Ordnerpfad übernommen.
         *  @param overwrite gibt an, ob Dateien, die bereits existieren überschrieben werden sollen.
         */
        bool appendFile(QString sourceAbsolutePath, QString targetRelativePath = NULL, bool overwrite = true);

        bool appendFile(const QByteArray &source, QString targetRelativePath, quint64 mtime = 0, bool overwrite = true);
		
        //! Fügt ein QByteArray an die Datei an und erstellt die Map neu (muss einen gültigen Header enthalten)
        void appendRaw(const QByteArray &source);

        //! Siehe appendFile(). Es werden nur einzelne Dateien überschrieben, nicht der gesamte Ordner.
        bool appendFolder(QString sourceAbsolutePath, QString targetRelativePath = QString(), bool overwrite = true);

        //! Benennt eine Datei oder einen Ordner um.
        /*!
        *  ACHTUNG: Überprüft momentan in keiner Weise, ob das Umbenennen Sinn macht(z.B. bereits vorhandener Dateiname etc.)
        *
        *  Prüft erst ob ein Datei Pfad übergeben wurde. Versucht andernfalls einen Order umzubennen.
        *  Gibt true zurück, wenn Dateien umbenannt wurden.
        */
        bool rename(QString oldPath, QString newPath);

        //! Speichert alle Veränderungen an der .pjob-Datei ab. Gibt true zurück, falls erfolgreich.
        void flush();

        //LESEN
        //! Gibt true zurück, falls sich eine Datei mit Namen relativePath im .pjob-File befindet
        bool contains(QString relativePath);

        //! Gibt true zurück, falls sich eine Datei im .pjob-File befindet, die mit relativePath beginnt
        bool containsDirectory(QString relativePath);

        //! liest eine Datei relativePath aus der .pjob-Datei aus und gibt diese als Bytearray zurück. Die Datei wird nicht gelöscht!
        QByteArray readFile(QString relativePath);

        //! liest eine Datei mit Namen relativePath inklusive zugehörigem Header ein und gibt sie als QBytearray zurück
        QByteArray readRaw(QString relativePath);

        //LÖSCHEN
        //! entfernt eine Datei relativePath aus dem .pjob-File. Gibt true zurück, falls erfolgreich. Muss über flush() gespeichert werden
        bool removeFile(QString relativePath);

        //! Löscht eine Datei oder einen Ordner mit dem übergebenen Pfad
        bool remove(QString relativePath);

        //ENTPACKEN
        //! entpackt eine oder mehrere Dateien aus dem .pjob-File in einen Ausgewählten Ordner
        /*!
         *  @param targetAbsolutePath gibt an, wohin die Dateien entpackt werden sollen. Falls NULL, wird Verzeichnis der .pjob-Datei entpackt.
         *  @param sourceRelativePath gibt an, welche Datei/welcher Ordner entpackt werden soll. Falls NULL, werden alle Dateien entpackt.
         *  @param overwrite gibt an, ob die Dateien beim Entpacken überschrieben werden sollen.
         */
        void extract(QString targetAbsolutePath = NULL, QString sourceRelativePath = NULL, bool overwrite = true);

        //INFO
        //! Gibt eine QStringList zurück, welche alle Dateinamen (relativer Pfad) im .pjob-File enthält
        QStringList content() const;

        //! Gibt eine detaillierte Liste mit den Dateien in der .pjob-File und deren Eigenschaften zurück.
        /*!
        *  Rückgabeformat ist für jede Datei eine Liste aus QVariants mit 3 Elementen. Die Elemente entsprechen
        *  in der folgenden Reihenfolge: fileName(QString), modificationTime(quint64), fileSize(int)
        */
        QList<QList<QVariant> > detailedContents(QString folder = "");

        //!Gibt die aktuelle Version zurück
        static int version();

signals:
    void output(QString fileName);
    void fileAdded(QList<QVariant> fileProperties);

private:
        struct intPair
        {
            int position, size;
        };

        //Hilfsfunktionen für Konstruktor und flush()
        bool isValid(); //**
        void createNewFile();
        void writeHeaderInformation(bool overwriteOldHeader);
        void map();
        QMap<QString,intPair> map(const QByteArray&, bool skip_header = true);

        //Hilfsfunktion für einfacheren Zugriff auf m_map
        void addToMap(QString relativePath, int position, int size);

        //Hilfsfunktion zum Anpassen von Pfadeingaben (wird nicht benutzt/benötigt)
        static QString adjustPath(QString path);

        //Hilfsfunktion zum Überprüfen, ob beim Hinzufügen einer Datei Kollisionen mit dem Windows-Dateisystem entstehen könnten
        bool proofUnique(QString relativePath);

        //Integer lesen und schreiben
        static void writeInt32(quint32 input, QByteArray &array, int pos);
        static void writeInt64(quint64 input, QByteArray &array, int pos);
        static quint32 readInt32(const QByteArray &array, int pos);
        static quint64 readInt64(const QByteArray &array, int pos);

        //Hilfsfunktion zum Potenzieren
        static quint32 power(const int base, int exponent);

        //Member
        QString m_path;
        uint m_version;
        bool m_modified;

        QByteArray m_data;
        QMap<QString,intPair> m_map;
};
