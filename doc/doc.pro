# Dummy, just to get custom compilers to work in qmake
TEMPLATE = lib
TARGET = Dummy

# .tex files to build
TEX += PJobFormatSpec.tex PQueue_Manual.tex

# Compiler for pdfs
doc_builder.name = tex2pdf
doc_builder.input = TEX
doc_builder.output = ${QMAKE_FILE_BASE}.pdf
doc_builder.commands = pdflatex ${QMAKE_FILE_BASE}

# This makes the custom compiler run before anything else
doc_builder.CONFIG += target_predeps

doc_builder.variable_out = documents.files
doc_builder.clean = ${QMAKE_FILE_BASE}.pdf \
                    ${QMAKE_FILE_BASE}.aux \
                    ${QMAKE_FILE_BASE}.toc \
                    ${QMAKE_FILE_BASE}.log
QMAKE_EXTRA_COMPILERS += doc_builder

# Install documentation
#documents.path = $$PREFIX/share/doc/
# If you don't specify this, all files must exist when you run qmake or else they will
# just silently be ignored
#documents.CONFIG += no_check_exist

#INSTALLS += documents
