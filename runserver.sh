data="true"
echo "Bringing Up Server"
while [ "$data" == "true" ]
do
./radi8d -p 1028 --nodebug
DATE=`date`
echo "[ERR]Server Crash:$DATE" >> radi8d.motd
sleep 60
echo " "
done
