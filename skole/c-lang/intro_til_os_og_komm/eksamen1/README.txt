###############################################
	Hjemme-eksamen 1. INF1060
	Rapport.
	Kandidatnr.: 356
###############################################

Oppgave:
	ifish - en kommandotolker for Linux.

Stegvis beskrivelse ref oppgave tekst:
(Ellers utfyllende kommentarer i kildefilene)


##	DEBUG:
	
-fungerte:
	Debug makro ble implementert i egen debug.h og fungerte etter ønske.

-fungerte ikke:
	Gjentatte forsøk på å få satt -DDEBUG gjennom gcc ble gitt opp.


##	Kompilering av koden:

	-Makefile:
	Makefilen som er brukt er en oppgavejustert makefil jeg bygget videre på
	etter forrige oblig. Lærdommen da var at make gjør livet lettere. GNU make 
	manualen på www.gnu.org ble studert og det ble implementert en del av det
	som virket smart og lettfattelig i en egendefinert make-mal.


	

##	Flere kildefiler.

	En annen lærdom fra forrige oblig var å separere kildefilene. Dette ble gjort
	med stort hell også i denne oppgaven og ble forsøkt designet etter ett mønster:

	-ifish.c:
		Her er grunnstammen i programmet. ifish.c inneholder main(). Her initialiseres
		shellet, prompt gis brukeren og kommandoer fra brukeren fordeles i stor del til
		funksjoner i de andre kildefilene.

	-shproc.c (shell processing):
		I denne filen finner man funksjonene som tolker/prosesserer kommunikasjonen 
		mellom bruker og ifish. 

		-Her tømmes stdin-buffer ved overflow.	
		-Her splittes strengen brukeren skriver.
		-Her ligger algoritmen for de ifish-spesifike kommandoene.
		-Her printes inneholdet i params-vektoren.
		-Samt funksjonen for de-allokering av paramvektoren ligger her,

	-unixcmd.c (unix command):
		I denne filen finner man håndteringen av kommunikasjon mellom
		ifish og operativsystemet.
		
		-Her hentes "USER" og "PATH" -variablene.
		-Path splittes opp her.
		-Forking av prosessen og eksekvering av kommando skjer her.
		-Sam funksjonen for å de-allokere path-vektoren er her.

	
	Forøvrig ble det til oppgaven "h - eksekveringshistore" implementert følgende filer,
	nærmere forklart siden:
	historyhandler.c, historyslave.c, linkhist.c med tilhørende mapHistory.h

	
	Kommentar til deloppgaven:
		Sett i ettertid ville jeg ha designet denne delen litt annerledes. Jeg ville gjort som jeg gjør
		i history-delen å ha en header fil for alle tree kildefilene, eller i allefall en ekstra styrende 
		ifish.h fil som inneholdt #defines og globale variable spesifikk til ifish. Dette fordi det ble litt
		uoversiktlig og på grensen til rotete slik det ser ut nå. 




##	Lese kommando fra tastaturet:

	Under denne oppgaven skal alt være implementert etter beskrivelsen gitt av oppgaveteksten.

	Kommentarer til deloppgaven:
		-fgets: Litt usikkerhet oppstod rundt det å anta at linjen aldri er lengere enn 120 tegn. Jeg tok en avgjørelse
		om å tømme stdin hvis den overskred 120 tegn ved hjelp av en getch i en tom while-loop. Dette ga også den
		fordelen at jeg slapp rekyl fra uleste meldinger i stdin gjennom ifish når jeg provoserte frem bugs.
		Alternativet jeg fant til denne løsningen var fflush(), men denne har udefinert oppførsel på stdin, i allefall 
		under Linux slik jeg har forstått det og ble forkastet.

		-Når brukeren trykker Ctrl+D er det lagt inn funksjonalitet for å frigjøre minnet allokert underveis.




##	Dele opp den innleste linjen i ord - kommando med parametere:
	
	Under denne oppgaven skal alt være implementert etter beskrivelsen gitt av oppgaveteksten.

	Kommentarer til deloppgaven:
		-isspace: Denne innebygde funksjonen ble brukt flittig under oppdeling av linjen og algoritmen som brukes
		synes å gjøre jobben. NULL-peker leges til elementet etter siste allokerte ord.




##	Utføre kommandoer:

	Under denne oppgaven skal alt være implementert etter beskrivelsen gitt av oppgaveteksten.*

	Kommentarer til deloppgaven:
		-exit/quit: Denne oppgaven lot meg for første gang bruke enum, og sette meg inn i hva enum er.
		Det gjorde det slik at jeg syns kommentarer var overflødig for switch-statementen som styrer om exit
		eller quit er kommandert. Jeg våger påstanden om at den er forstålig slik den fremstår.
		Alt av minne bli de-allokert ved kommandering av exit eller quit.

		-Linje uten ord: Ny prompt med nytt kommando-antall gis.

		-Starte en barneprosess: Denne deloppgaven brukte jeg litt tid på å finne en måte som ga færrest 
		mulig overflødige tester for å finne riktig-path. Til slutt fant jeg funksjonen stat() som sjekker
		om det argumentet er en eksekverbar fil og hvis så returnerer null. Dette gjorde at jeg slapp
		å forke unødvendige prosesser.

		-&: Test av '&' overlot jeg til funksjonen parse_cmd() som fjerner & fra strengen og setter et globalt
		flagg som indikerer &/ikke-&. Når dette flagget er 1 velges WNOHANG-option i waitpid() ellers 0.
		Dette gjør at mor ikke venter på barn.
		  Hadde intensjoner om å implementere kontroll av fildeskriptorene for barne-prosessen for å 
		ikke få overskrevet prompten ved barnets retur. Prøvde en løsning, men fikk den ikke til å fungere
		så droppet det.


		* Med et lite forbehold. Jeg valgte å bruke env-vectoren som argument til execve() i stedet for
		å bygge opp egen for formålet. Dette skyltes at jeg hadde surret bort path-variablen etter å ha 
		splittet den opp. Jeg valgte en litt dum måte å håndtere path-variablen, ved bl.a å allokere
		den i minnet. Dette viste seg å være ganske grunnløst og lite hensiktsmessig, men det virket
		smart den gang funsksjonen ble opprettet (ikke nå).



##	Innebygde kommandoer:

	Under denne oppgaven skal alt være implementert etter beskrivelsen gitt av oppgaveteksten.

	kommentarer til deloppgaven:
		Utover 'h' er også 'exit' 'quit' og 'type' definert under type-kommandoen da jeg fant det natrulig.
		Skrives bare 'type' får brukeren prompten i retur, likt bash.


##	HISTORY:

	
	-Innledning:
		I denne delen av oppgaven ble det gjort en del endringer i oppsettet, lært av feil fra foregånde del. 
		-Blant annet valgte jeg å ha en sammenfattende header-fil. 
		-Jeg forstod at det ville være lurt å gjøre koden lett å utvide ved eventuelle endringerved å flittig 
		bruke define-verdi der operasjoner var direkte avhengig av størrelsene på bitmappet og history buffer.
		-Den fra før litt ullene Ternary-statementet ble endelig forstått og implementert der det var fornuftig.

	
	-Implementasjon:

		Denne delen er delt opp i tre filer pluss en header-fil.

		-historyhandler.c:
			Denne filen håndterer innsetting i history buffer, sletting og eksekvering av historiske kommandoer,
			samt initialisering av bitmap og history buffer til null. Her ligger også debug representasjon av 
			bitmap og historybuffer.
		-historyslave.c:
			Mellommmannen i history-prosessen.
			Her er hjelpefunksjonene handleren trenger for å legge kommandoer i minnet. Og det er her linkhist.c
			går for å slette sine.
		-linkhist.c:
			Denne filen inneholder alle funksjoner knyttet til håndteringen av den lenkede listen.
	



##	'h'-eksekveringshistorie:
	
	Under denne oppgaven skal alt være implementert etter beskrivelsen gitt av oppgaveteksten.

	kommentarer til deloppgaven:
		Oppgaven ble løst ved, som oppgave sier, å lage en lenket liste. Jeg valgte å kunne entre listen
		fra to sider, head og tail, viss navn virker å være standard. Dette gjør visse operasjoner litt enklere
		og ikke minst vil tiden det tar å gå igjennom listen kuttes betraktlig. For eksempel ved at programmet 
		sletter fra head og setter inn fra tail unngår man å traversere hele listen ved innsetting eller sletting
		av eldste.
		-History numerene og det totale antallet fås ved traversering av hele listen. Det er også lagt inn en
		en-tallsform ved tallet 1 med ternary-statement.




## 	Håndtering av minnet for history:
		
	Under denne oppgaven skal alt være implementert etter beskrivelsen gitt av oppgaveteksten.*

	kommentarer til deloppgaven:
		-Brukte en god stund på å finne en antatt rask og god måte å telle ledige blokker i bitmapen. Det 
		kan som beskrevet under bugs lenger ned på siden, ha feilet. Endte opp med to muligheter, hvorpå en
		brukes til å finne blokker og den andre er implemeter som teller for debug-funksjonen.
		-Til å finne ledige blokker brukte jeg en løsning jeg hadde fra en tidligere oppgave fra "the C 
		Programming language" oppgave 2-9 faster bit count og snudde på retur-verdien.
		-Til all "nulling"-av minnet er det brukt memset, da bzero er deprecated, merket som legacy
		og ble fjernet fra posix standaren i 08.

		*Se forøvrig ytterligere kommentarer under Bugs under.




##	Metadata-struktur:
		Forklart under punktet 'h'-eksekvering.



		
##	Debug bitmap og Datablocks:
	
	Under denne oppgaven skal alt være implementert etter beskrivelsen gitt av oppgaveteksten.
	
	Kommentarer til deloppgaven:
		Virker slik som eksempelt viser bortsett fra siste bitet (Forklart under Bugs)
		To, kanskje overdrevent uleselige ternary-operatorer styrer prosessen, dog var det
		gøy og utfordrene å lage dem.




##	'h i' - utfør den i'te yngste historiekommandoen

	Under denne oppgaven skal alt være implementert etter beskrivelsen gitt av oppgaveteksten.

	kommentarer til deloppgaven:
		En del forutsettninger v.h.a if-setninger måtte til for å få denne kommandoen til å oppføre seg
		pent, tror alle mulige fallgruber er tatt hensyn til, samt frigjøring av minnet allokert av kommandoen	
		og den exekverte kommandoen.




##	'h -d i' - slette oppføring i kommandohistorien
		

	Under denne oppgaven skal alt være implementert etter beskrivelsen gitt av oppgaveteksten.

	kommentarer til deloppgaven:
		Som for 'h i' var det en del forutsettniger som skulle til. Ellers var det en 
		oppgave som kunne lene seg på tidligere implementerte funksjoner.




## Til slutt:
	Programet er testet under Valgrind for minnelekasjer og Valgrind fant ingen.

		
	
Bugs (Kjente):

1. Allokering av de siste åtte bitene i History buffer (Bit 63 i bitmap).

	Beskrivelse:
		Funksjonen find_free_blocks() nekter å gjennomføre 7. iterasjon på 7. byte,
		noe som leder til at byte 504-511 ikke blit allokert i historyBuffer.

	Mulige årsaker/løsninger:
		Foreløpig uvisst årsak. 

		- Var inne på tanken om at det kanskje kunne skyldes en pre-
		prosesserings-optimalisering gjort av gcc for å hindre at carry-bit blir overført
		til minne ifish ikke eier, siden det er en OG-operasjon på potensielt satt siste-bit.
		Dette slo jeg dog fra meg da jeg endret programmet til at funksjonen print_bitmap()
		gjorde jobben med å telle ledige bits istedet. Fikk nøyaktig samme resultat som før.

		- print_bitmap() ser null i bit 63, antar att dette korrekt, derfor er ikke problemet
		at bitet er satt bak ryggen min.

		- En annen mulighet jeg har vært inne på er en forlengelse av første punkt. Rett og slett
		at programmet lar carry overflowe helt til siste bit og carry ikke kan flyttes lenger.
		Dette skulle da lede til at print_bitmap() ser 1 i 63 bit, noe som sagt den ikke gjør.

		- Siste og mest sannsynlig. Jeg har en logikk-feil ett eller annet sted som jeg har
		sett meg blind på.

	Konklusjon:
		Dette er en hard nøtt å knekke. Antagelig er det noe helt basalt jeg har oversett
		i logikken min og alle forsøk på retting har vært å angripe alt annet enn det som
		leder til feilen. Det er lett å bekrefte at det man har gjort stemmer når man 
		retter feil man selv har initiert..



2. Ulovlig skriving til history buffer.

	Beskrivelse:
		Under visse omstendigheter får jeg provosert frem at bytes i history buffer
		overflytes ved lange kommandoer. Det er ingen spesiell logikk i hva som overskrives
		så langt jeg kan se. Det vil si, det kan dukke opp enkelte ord fra midten av en 
		lang streng spredd tilsynelatende vilkårlig inn i bufferet.

	Mulige årsaker/løsninger:
		Helt klart noen begrensninger som ikke er satt riktig og/eller feil med logikken 
		i algoritmen. 

	Konklusjon:
		Dette er en litt skummel feil, men jeg antar at den skyldes noen forutsetninger som
		ikke blir oppfylt p.g.a manglende begrensninger i koden.
		Antar at den er løsbar ved å justere koden slik den er skrevet.

 
