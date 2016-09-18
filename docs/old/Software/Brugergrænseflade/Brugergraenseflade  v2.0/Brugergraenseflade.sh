#!/bin/bash

. MenuFunktioner


hovedMenu
accessmenu="true"
while [ "$accessmenu" == "true" ]; do
  hovedMenu
  read yourch
  case $yourch in
	1) accessTest="true"
	      while [ "$accessTest" == "true" ]; do
		#./file_client J KommunikationsData/getMomentSetpunkt.txt
		#./file_client Q KommunikationsData/getTestStatus.txt
		#./file_client D KommunikationsData/getLiveData.txt
	      	testMoment
       		echo ""
		echo "Teststatus:" `cat KommunikationsData/getTestStatus.txt` 
		echo "Setpunkt - Moment:" `cat KommunikationsData/getMomentSetpunkt.txt` " Nm"
		read testch
       			case $testch in
				1) echo "";
				     ./Test_kontrol L #Oeg moment +1Nm
				     ./file_client J KommunikationsData/getMomentSetpunkt.txt
				     ;;
				2) echo "";
				     ./Test_kontrol N #Saenk moment -1Nm
				     ./file_client J KommunikationsData/getMomentSetpunkt.txt
				     ;;
				3) echo "";
				     ./Test_kontrol K #Oeg moment +0.1Nm
				     ./file_client J KommunikationsData/getMomentSetpunkt.txt
				     ;;		
				4) echo "";
				     ./Test_kontrol M #Saenk moment -0.1Nm
				     ./file_client J KommunikationsData/getMomentSetpunkt.txt
				     ;;	
				5) echo "";
				     ./Test_kontrol O #Start test
				     ./file_client Q KommunikationsData/getTestStatus.txt
				     ;;	
				6) echo "";
				     ./Test_kontrol P #Stop test
				     ./file_client Q KommunikationsData/getTestStatus.txt
				     ;;	
				7) echo "";
				     ./file_client D KommunikationsData/getLiveData.txt
				     echo "Live-data:"
				     cat KommunikationsData/getLiveData.txt 
				     echo "";
           			     echo "Tryk paa Enter for at fortsaette" ; read ;;
				8) accessTest="false" ;;
				*) echo "Ugyldig indtastning - Vaelg menu [1-8]"; echo "";
	    			 echo "Tryk paa Enter for at fortsaette" ; read ;;
	 		esac
     	done ;;
	2) accessGemData="true"
		while [ "$accessGemData" == "true" ]; do
		gemDatahistorik
		read gemdatach
       			case $gemdatach in
				1) echo "";
				     ./file_client F Datahistorik/Moment_Historik.txt 		#Hent Momenthistorik
				     ./file_client E Datahistorik/Hastighed_Historik.txt 	#Hent Hastighedshistorik
				     ./file_client G Datahistorik/P_Mekanisk_Historik.txt	#Hent Mekanisk Effekt historik
				     ./file_client H Datahistorik/P_Elektrisk_Historik.txt	#Hent Elektrisk Effekt historik
				     ./file_client I Datahistorik/Virkningsgrad_Historik.txt	#Hent Virkningsgradshistorik
				     ./file_client R Datahistorik/Energiforbrug_Historik.txt	#Hent Energiforbrugshistorik
				     echo "Data er gemt i mappen Datahistorik"
				     ;;
				2) accessGemData="false" ;;
				*) echo "Ugyldig indtastning - Vaelg menu [1-2]"; echo "";
				 echo "Tryk paa Enter for at fortsaette" ; read ;;
	 	esac
        done ;;
	3) accessmenu="false" ;;
	*) echo "Ugyldig indtastning - Vaelg menu [1-3]"; echo "";
	 echo "Tryk paa Enter for at fortsaette" ; read ;;
esac
done
