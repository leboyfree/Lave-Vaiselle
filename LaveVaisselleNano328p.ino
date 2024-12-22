

#include <LiquidCrystal.h>

//equipement lave linge
#define bpMarcheArret A5
#define bpSelection A4
#define debimetre 2
#define pressotat A3
#define antidebordement A2
#define securitePorte A1
#define chauffe 13
#define electrovanne 10 
#define pompeVidange 11
#define pompeLavage 12
#define Produit 9

#define CTNpin A0  //definition broche analogique CTN
float Rpont = 20000; //Valeur resistance du pont CTN

//afficheur
const int rs = 8, en = 7, d4 = 6, d5 = 5, d6 = 4, d7 = 3;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


volatile long debmComptage = 0;     // Pour comptage des impulsions
long valeurRemplissage =5;     // valeur pour remplissage debimetre

//variables temps demandées
unsigned long tempsLavage = 0;//3h en millisecondes
unsigned long tempsLavageRapide = 30000;
unsigned long tempsLavageEco = 20000;
unsigned long tempsLavageInt =20000;
unsigned long tempsRemplissage = 10000; // 20s en millisecondes
unsigned long tempsVidange = 10000; // 2mn en millisecondes
unsigned long tempsMaxiChauffe = 20000; // 15 mn en millisecondes
unsigned long tempsPreRincage = 30000; //prerinçage à froid
unsigned long tempsRincage = 30000;//rinçage à chaud
unsigned long tempTotalMaxiLavage = 1000000;
//variables température demandées
unsigned long temperatureLavage = 30;
unsigned long  temperatureRapide = 30;
unsigned long temperatureRincage= 30;
unsigned long temperatureEco = 40;
unsigned long temperatureInt = 60;

//variable retour mesure température
float temperature = 0; 
//Variables départs des tempos
unsigned long departTotalMaxiLavag =0, departTempsLavage =0 ,departTempsRemplissage =0,departTempsVidange =0, departTempsMaxiChauffe =0;
unsigned long tempoMesureTemperature = 0; 
//bool verrouillage fonctionnement lavage
bool onPrerincage = false,onRincage = false,onPrelavage = false,onProgrammeRapide = false, onProgrammeEco = false,onProgrammIntensif =false,onCycleLavage =false, onJetsLavage =false;
bool onMenuReglage = false, onMenuStart = false, onReglage = false, onRemplissage = false, onChauffe = false, onPrevidange = false, onProduit= false,onVidange = false,onArret = false, onPause = false;


//variables switch
 byte SelectionMenu = 0, sequenceLavage = 0, ligne1Affichage =0, ligne2Affichage =0; 
//Tempos

 //comptage des impultions
void debmImpulsionInterrupt()
{
	debmComptage ++;
}

void setup()
{

	pinMode(debimetre, INPUT_PULLUP);     // Sans résistance externe
	//pinMode(debimetre, INPUT);     // Avec résistance externe
	attachInterrupt(digitalPinToInterrupt(debimetre), debmImpulsionInterrupt, FALLING);

	pinMode(bpMarcheArret,INPUT_PULLUP);
	pinMode(bpSelection,INPUT_PULLUP);
	pinMode(debimetre,INPUT_PULLUP);
	pinMode(pressotat,INPUT_PULLUP);
	pinMode(securitePorte,INPUT_PULLUP);
	pinMode(antidebordement,INPUT_PULLUP);
	pinMode(chauffe,OUTPUT);
	pinMode(electrovanne, OUTPUT);
	pinMode(pompeVidange, OUTPUT);
	pinMode(pompeLavage, OUTPUT);
	
	digitalWrite(chauffe, LOW);
	digitalWrite(electrovanne, LOW);
	digitalWrite(pompeVidange, LOW);
	digitalWrite(pompeLavage, LOW);
	
	lcd.begin(16, 2);//initialisation affichage		
	ligne1Affichage =1;ligne2Affichage =1; affichage();//affichage démarrage	

	} 

	
void remplissage()
{

if (digitalRead(electrovanne)== LOW && digitalRead(pressotat) == HIGH && debmComptage < valeurRemplissage){
	digitalWrite(electrovanne, HIGH);
	ligne1Affichage = 2; ligne2Affichage =5;affichage();
	 }
if ((millis()-departTempsRemplissage >tempsRemplissage)||digitalRead(pressotat) == LOW ||debmComptage > valeurRemplissage){
	digitalWrite(electrovanne, LOW);ligne1Affichage = 2; ligne2Affichage =2;
	affichage();onRemplissage = false;sequenceLavage++;onCycleLavage = true;
	delay(500);
		} 	
	}
	
void produitLavage()
{

	digitalWrite(produitLavage, HIGH);
	delay(2000);
	digitalWrite(produitLavage, LOW);
	ligne1Affichage = 17; ligne2Affichage =9;affichage();
	onProduit = false;sequenceLavage++;onCycleLavage = true;
	delay(500);

	}

void vidange(){
	unsigned long tempsPrevidange = tempsVidange/3;
if (digitalRead(pompeVidange)== LOW){
	digitalWrite(pompeVidange, HIGH);
	if (onPrevidange){ligne1Affichage = 15;ligne2Affichage =5;affichage();}
	else {ligne1Affichage = 4;ligne2Affichage =5;affichage();}	
			
	 }
if ((millis()-departTempsVidange >tempsVidange)||(onPrevidange && (millis()-departTempsVidange>tempsPrevidange))){
	digitalWrite(pompeVidange, LOW);lcd.clear();
	
	if (onPrevidange){ligne1Affichage = 15; ligne2Affichage =2;affichage();sequenceLavage++;onCycleLavage = true;
		onPrevidange =false;onVidange = false;		
		}
	else{ligne1Affichage = 4; ligne2Affichage =2;affichage();sequenceLavage++;onCycleLavage = true; onVidange = false;}
	
	delay(500);
		}
	}

int mesureTemperature(){
	float N = analogRead(CTNpin); //mettre la valeur de la CTN dans N
	float Url = 5*N/1023;   //Converti la valeur en tension sue resistance ligne
	float Rctn = Rpont*(5-Url)/Url;//Calcul valeur résitance ctn
	float Temperature = -26.04*log(Rctn)+265.26;//calcul temperature et convertion en °c  
	return Temperature;
	}
	
void chauffage(){
		
	if(millis() - 2000>(tempoMesureTemperature)){temperature = mesureTemperature();tempoMesureTemperature = millis();
		if (sequenceLavage ==7){ligne1Affichage = 13;ligne2Affichage =7;affichage();}
		else if (sequenceLavage ==10){ligne1Affichage = 14;ligne2Affichage =7;affichage();}
		else{ ligne1Affichage = 8;ligne2Affichage =7;affichage();}
		}		
		
				     
	if (digitalRead(chauffe)== LOW && temperature <(temperatureLavage - 5)){departTempsMaxiChauffe = millis();
	digitalWrite(chauffe, HIGH);}
	 
	

if ((millis()-departTempsMaxiChauffe >tempsMaxiChauffe )||temperature > temperatureLavage){
	digitalWrite(chauffe, LOW);
	
	}
}


void JetsLavage(){
	
if (digitalRead(pompeLavage)== LOW && digitalRead(antidebordement)== HIGH && digitalRead(securitePorte)== LOW){
	digitalWrite(pompeLavage, HIGH);
	if (sequenceLavage == 7){ligne1Affichage = 13;affichage();}
	else if(sequenceLavage == 10){ligne1Affichage = 14;affichage();}
	else {ligne1Affichage = 8;affichage();}	
	 }
	 
	if((digitalRead(antidebordement)== LOW || digitalRead(securitePorte)== HIGH) && digitalRead(pompeLavage)== HIGH){
	digitalWrite(pompeLavage, LOW);
	if(digitalRead(antidebordement)== LOW){lcd.clear();ligne1Affichage = 10; ligne2Affichage =5;affichage();delay(500);}
	if (digitalRead(securitePorte)== HIGH){lcd.clear();ligne1Affichage = 12; ligne2Affichage =6;affichage();delay(500);}
	}
	
if (millis()-departTempsLavage >tempsLavage - 600000){onChauffe = false;}	 
if (millis()-departTempsLavage >tempsLavage){	
	digitalWrite(pompeLavage, LOW);digitalWrite(chauffe, LOW);	
	if (sequenceLavage == 7){ligne1Affichage = 13;ligne2Affichage =2;affichage();}
	else if(sequenceLavage ==10){ligne1Affichage = 14;ligne2Affichage =2;affichage();}
	else {ligne1Affichage = 8;affichage();}	
	onJetsLavage = false;onChauffe = false;sequenceLavage++;onCycleLavage = true;
	
	delay(500);
		}	
	}

//cycle lavage 	
void cycleLavage(){
	if (sequenceLavage>12)sequenceLavage=0;
	switch(sequenceLavage)  {
	case 0:{onCycleLavage = false ;delay(10);break;}
	case 1:{onPrevidange = true;onVidange = true;departTempsVidange = millis();departTotalMaxiLavag = millis();onCycleLavage = false ;delay(10);break;}
	case 2:{onRemplissage = true ;departTempsRemplissage = millis();onCycleLavage = false ;delay(10);break;}	
	case 3:{onProduit = true;onCycleLavage = false ;delay(10);break;}
	case 4:{onJetsLavage = true;onChauffe = true;departTempsLavage = millis();departTempsMaxiChauffe = millis();onCycleLavage = false;delay(10);break;};
	case 5:{onVidange = true;departTempsVidange = millis();onCycleLavage = false ;delay(10);break;}
	case 6:{onRemplissage = true ;departTempsRemplissage = millis();onCycleLavage = false ;delay(10);break;}
	case 7:{onJetsLavage = true;departTempsLavage = millis();tempsLavage = tempsPreRincage;onCycleLavage = false;delay(10);break;};
	case 8:{onVidange = true;departTempsVidange = millis();onCycleLavage = false ;delay(10);break;}
	case 9:{onRemplissage = true ;departTempsRemplissage = millis();onCycleLavage = false ;delay(10);break;}
	case 10:{onJetsLavage = true;onChauffe = true;departTempsLavage = millis();tempsLavage = tempsRincage;departTempsMaxiChauffe = millis();onCycleLavage = false;delay(10);break;};
	case 11:{onVidange = true;departTempsVidange = millis();onCycleLavage = false ;delay(10);break;}
	case 12:{onArret = true;departTempsLavage = millis();onCycleLavage = false ; onRemplissage = false; onChauffe = false; onJetsLavage = false;onVidange = false;delay(10);break;}
	
	}
	
	
	}	

	
void affichage(){
	
    lcd.clear();
	//ligne 1 affichage	
	if (ligne1Affichage > 0){
	switch(ligne1Affichage) { 	
	 	case 1:{ lcd.setCursor( 1, 0 );lcd.print("Lave vaiselle");delay(200);ligne1Affichage =0; break;}
	case 2:{ lcd.setCursor( 2, 0 );lcd.print("Remplissage");delay(200);ligne1Affichage =0;break;}
	case 3:{ lcd.setCursor( 3, 0 );lcd.print("Chauffe");delay(200);ligne1Affichage =0;break;}
	case 4:{ lcd.setCursor( 4, 0 );lcd.print("Vidange");delay(200);ligne1Affichage =0;break;} 
	case 5:{ lcd.setCursor( 2, 0 );lcd.print("Prog rapide");delay(200);ligne1Affichage =0;break;}
	 case 6:{ lcd.setCursor( 3, 0 );lcd.print("Prog eco ");delay(200);ligne1Affichage =0;break;}
	 case 7:{ lcd.setCursor( 3, 0 );lcd.print("Prog Int ");delay(200);ligne1Affichage =0;break;}
	 case 8:{ lcd.setCursor( 5, 0 );lcd.print("Lavage ");delay(200);ligne1Affichage =0;break;} 
	 case 9:{ lcd.setCursor( 4, 0 );lcd.print("Arret");delay(200);ligne1Affichage =0;break;}
	 case 10:{lcd.setCursor( 0, 0 );lcd.print("Antidebordement");delay(200);ligne1Affichage =0;break;}	
	 case 11:{lcd.setCursor( 3, 0 );lcd.print("Programme");delay(200);ligne1Affichage =0;break;}
	 case 12:{lcd.setCursor( 4, 0 );lcd.print("Porte");delay(200);ligne1Affichage =0;break;}
	  case 13:{lcd.setCursor( 3, 0 );lcd.print("Prerincage");delay(200);ligne1Affichage =0;break;}
	   case 14:{lcd.setCursor( 4, 0 );lcd.print("Rincage");delay(200);ligne1Affichage =0;break;}
	   case 15:{lcd.setCursor( 2, 0 );lcd.print("Previdange");delay(200);ligne1Affichage =0;break;}
	   case 16:{lcd.setCursor( 1, 0 );lcd.print("Tps depasse!");delay(200);ligne1Affichage =0;break;}
	   case 17:{lcd.setCursor( 3, 0 );lcd.print("Produit");delay(200);ligne1Affichage =0;break;}
	}
		}
if (ligne2Affichage > 0){	
	//ligne 2 affichage
	switch(ligne2Affichage) {
	case 1:{lcd.setCursor( 4, 1 );lcd.print("en route ");delay(200);ligne2Affichage =0;break;}
	case 2:{lcd.setCursor( 5, 1 );lcd.print("fini!");delay(200);ligne2Affichage =0;break;}
	case 3:{lcd.setCursor( 2, 1 );lcd.print("selectionne");delay(200);ligne2Affichage =0;break;}
	case 4:{lcd.setCursor( 2, 1 );lcd.print("effectue");delay(200);ligne2Affichage =0;break;}
	case 5:{lcd.setCursor( 3, 1 );lcd.print("en cours");delay(200);ligne2Affichage =0;break;}
	case 6:{lcd.setCursor( 3, 1 );lcd.print("Ouverte");delay(200);ligne2Affichage =0;break;}
	case 7:{lcd.setCursor( 4, 1 );lcd.print(temperature);lcd.print("*C");ligne2Affichage =0;break;}
	case 8:{lcd.setCursor( 3, 1 );lcd.print("Erreur!");ligne2Affichage =0;break;}
	case 9:{lcd.setCursor( 3, 1 );lcd.print("Lavage");ligne2Affichage =0;break;}
	 }
	}

}
			
void menuReglage(){
		  
		  	   delay(100);SelectionMenu++;
		      if (SelectionMenu>4)SelectionMenu=0;
		      if (SelectionMenu== 0)lcd.clear();
			      switch(SelectionMenu)  {
				  
			     
			     case 1:{ligne1Affichage = 5;ligne2Affichage =3;affichage();onMenuReglage = false;delay(10);break;}
			     case 2:{ligne1Affichage = 6;ligne2Affichage =3;affichage();onMenuReglage = false;delay(10);break;}
			     case 3:{ligne1Affichage = 7;ligne2Affichage =3;affichage();onMenuReglage = false;delay(10);break;}	 
			     case 4:{ligne1Affichage = 4;ligne2Affichage =3;affichage();onMenuReglage = false;delay(10);break;} 
		        
		        
			}

}

	
void menuStart(){
	
		switch(SelectionMenu){		
			
					
			case 1:{onProgrammeRapide = true;
					tempsLavage = tempsLavageRapide;temperatureLavage = temperatureRapide;sequenceLavage =1;onCycleLavage = true;		
					ligne1Affichage = 5;ligne2Affichage =5;affichage();onMenuStart = false;delay(100);break;}
				
			case 2:{onProgrammeEco = true;					
					tempsLavage = tempsLavageEco;temperatureLavage = temperatureEco;sequenceLavage =1;onCycleLavage = true;			
					ligne1Affichage = 5;ligne2Affichage =5;affichage();onMenuStart = false;delay(100);break;}
				
			case 3:{onProgrammIntensif = true;
				temperatureLavage = temperatureInt;onCycleLavage = true;sequenceLavage =1;onCycleLavage = true;
				tempsLavage = tempsLavageInt;ligne1Affichage = 7;ligne2Affichage =5;affichage();onMenuStart = false;delay(100);break;}
				
			case 4:{onProgrammeEco = true;
				temperatureLavage = temperatureInt;onCycleLavage = true;sequenceLavage =1;onCycleLavage = true;
				tempsLavage = tempsLavageInt;ligne1Affichage = 7;ligne2Affichage =5;affichage();onMenuStart = false;delay(100);break;}
				
			
	}
SelectionMenu = 0;
}


void arret(){
	digitalWrite(chauffe, LOW);
	digitalWrite(electrovanne, LOW);
	digitalWrite(pompeVidange, LOW);
	digitalWrite(pompeLavage, LOW);
	lcd.clear();
	ligne1Affichage = 11; ligne2Affichage =2;affichage();
	sequenceLavage =0;onCycleLavage = false;onRemplissage = false; onChauffe =false; onJetsLavage =false;onArret = false;
	while(digitalRead(bpSelection) == LOW && digitalRead(securitePorte == HIGH));
	}


void loop()
{




	//Dépassement tps maxi de lavage
	if( millis()-departTotalMaxiLavag >tempTotalMaxiLavage && sequenceLavage >0){
				
		onArret = true;
		ligne1Affichage = 16;ligne2Affichage =8;affichage();
		}
	//Bouton sélection programme	
	if (digitalRead(bpSelection) == LOW && digitalRead(securitePorte) == HIGH) {onMenuReglage = true;
		if (digitalRead(securitePorte) == HIGH){onMenuStart = false;}
		delay(30);
		}
	//Bouton arret
	if (digitalRead(bpMarcheArret) == LOW) {onArret = true;delay(30);}
	
	if (digitalRead(securitePorte) == LOW ) {
	 menuStart();onMenuStart = true;delay(30);}
				

		if (onMenuReglage){menuReglage();}
		if (onMenuStart){menuStart();}
		if (onArret){arret();}
		if (onRemplissage)	{remplissage();}
		if (onProduit){produitLavage();}
		if (onChauffe)	{chauffage();}		
		if (onVidange){vidange();}
		if (onJetsLavage) {JetsLavage();}		
		if (onCycleLavage){cycleLavage();}
}
