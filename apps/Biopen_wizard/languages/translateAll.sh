
#! bash file for translations in linux 
#! QT5_BINARY_DIR=/home/mauro_veas/Qt/5.11.1/gcc_64/bin
QT5_BINARY_DIR=/home/mauro/Qt/5.15.1/gcc_64/bin

${QT5_BINARY_DIR}/lrelease ./eng.ts ./eng.qm
${QT5_BINARY_DIR}/lrelease ./sve.ts ./sve.qm
${QT5_BINARY_DIR}/lrelease ./ita.ts ./ita.qm
${QT5_BINARY_DIR}/lrelease ./cn.ts ./cn.qm


read -p "press any key to exit ..."


