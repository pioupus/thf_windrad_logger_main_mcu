"git" log -1 --format="format:#define GITHASH 0x%h"  > include/vc.h
echo " " >> include/vc.h
"git" log -1 --date=short --format="format:#define GITDATE \"%ad\"" >> include/vc.h
echo " " >> include/vc.h
"git" log -1 --date=short --format="format:#define GITUNIX %ct" >> include/vc.h