(require r5rs)
;; Obligatorisk oppgave 1b
;; Inf2810
;; Christopher Storheim
;; Paa forhaand beklager manglende norske vokaler...

;;----------- Oppgave 1  ---------------------------------------------------



;;----A --------------------------------------------------------------------
(cons 47 11)
;; Bruk av cons (construct) indikerer at vi oensker et par
;; konstruert. Det som konstrueres er en singel-lenket liste av noder (par) som
;; inneholder peker til en verdi (car), og en peker til neste node i paret (cdr).
;; Siste node i paret peker videre til null/nil/'() og indikerer slutt.
;; Eksempel:
;;
;;     _car_cdr_     _car_cdr_
;;---->|_o_|_o-|---->|_o_|_\_|
;;       |             |
;;       |             |
;;       v             v
;;       47            11
;;
;; Her peker foerste felt i foerste node til 47, andre felt holder peker
;; til neste node i konstruksjonen. Her peker foerste felt til tallet 11
;; og neste felt peker paa null\'() indikert visuelt av '\'.



;;----B --------------------------------------------------------------------
(cons 47 '())
;; Her konses 47 med den tomme listen, altsaa null.  
;;     _car_cdr_
;;---->|_o_|_\_|
;;       |
;;       |
;;       v
;;       47
;;
;; Cdr-feltet til noden skal peke til den tomme listen. car skal peke til 47. 


;;----C --------------------------------------------------------------------
;;     _car_cdr_     _car_cdr_
;;---->|_o_|_o-|---->|_o_|_\_|
;;       |             |
;;       |             |
;;       v             v
;;       47            11
;;
;; (list 47 11) En liste er en sammenkobling av en eller flere noder
;; (sekvens av par)
;; 
;; Det kan vises ved aa bygge en liste opp ved aa konse elementene:
;; (list? (cons 47 (cons 11 '()))) => #t


;;----D ---------------------------------------------------------------------
;;
;;     _car_cdr_     _car_car_
;;---->|_o_|_o-|---->|_o_|_\_|
;;       |             |
;;       |             |
;;       v             v
;;       47          _car_cdr_     _car_cdr_
;;                   |_o_|_o-|---->|_o_|_\_|
;;                    |             |
;;                    |             |
;;                    v             v
;;                    11            12
;;
;; (47 (11 12)) Her blir paret (11 12) en indre liste i listen. 'Adresse-feltet'
;; til cdr av listen peker paa denne indre listen, altsaa vil car av cdr av
;; listen (cadr) vaere referansen til den andre listen.
;;


;;----E ---------------------------------------------------------------------
;; (define foo '(1 2 3))
;; (cons foo foo)
;;
;;     _car_cdr_       _car_cdr_   _car_cdr_   _car_cdr_
;;---->|_o_|_o-|------>|_o_|_o-|-->|_o_|_o-|-->|_o_|_\_|
;;       |               |           |           |
;;       |               |           |           |  
;;       |               v           v           v
;;       |               1           2           3
;;       |
;;       |
;;       v
;;     _car_cdr_   _car_cdr_    _car_cdr_ 
;;     |_o_|_o_|-->|_o_|_o_| -->|_o_|_\_|
;;       |           |            |      
;;       |           |            |
;;       v           v            v
;;       1           2            3
;;
;; Slik jeg tenker er at naar noe skal konses er foerste argument til cons
;; car og det neste er cdr.
;; Altsaa (cons 'hva skal car peke paa' 'hva skal cdr peke paa').
;;
;; Da faar jeg at car peker paa listen foo og cdr peker paa listen foo,
;; slik som illustrert over.



;;----F ----------------------------------------------------------------------
;;
;; Illustrert: trekke ut tallet 3 fra (1 2 3 4)
;;
;;     _car_car_     _car_cdr_     _car_cdr_     _car_cdr_
;;---->|_o_|_o-|---->|_o_|_o_|---->|_o_|_o-|---->|_o_|_\_|
;;       |             |             |             |
;;       |             |             |             |
;;       v             v             v             v
;;       1             2             3             4
;;
;; (cdr f):
;;_cdr_     _car_cdr_     _car_cdr_     _car_cdr_
;;|_o-|---->|_o_|_o_|---->|_o_|_o-|---->|_o_|_\_|
;;            |             |             |
;;            |             |             |
;;            v             v             v
;;            2             3             4
;;
;; (cdr (cdr f)):
;;_cdr_     _car_cdr_     _car_cdr_
;;|_o_|---->|_o_|_o-|---->|_o_|_\_|
;;            |             |
;;            |             |
;;            v             v
;; (car)---> (3)            4
;;
(define f '(1 2 3 4))
(car (cdr (cdr f)))
;; Alternativer:
(car (cddr a))
;; eller
(caddr a)
;; (car (cdr ( cdr a))) -> (car (cdr (2 3 4))) -> (car (3 4)) = 3
;; car henter ut foerste element, cdr gir resten av listen frem til '()



;;----G ---------------------------------------------------------------------
;;Trekke ut tallet 3 fra ((1 2)(3 4))
;;   
;;      _car g_                              _car_cdr_
;;---->|_o_|_o-|-----cdr g------------------>|_o_|_\_|
;;       |                                     |
;;       |                                     |
;;       v                                     v
;;     _car_cdr_     _car_cdr_             _car_cdr_     _car_cdr_
;;     |_o_|_o_|---->|_o_|_\_|             |_o_|_o-|---->|_o_|_\_|
;;       |             |                     |             |   
;;       |             |                     |             |
;;       v             v                     v             v
;;       1             2                     3             4
;;

;; Beskrivelse:
;; Her har vi et tilfellet av liste av lister hvor (cdr g) peker paa
;; resten av strukturen etter car (indikert med pil).
;;
;; Dette blir den indre listen inneholdene paret (3 4), siden det ikke er noen
;; peker videre fra 'Content of Decrement Register' tilhoerende denne noden.
;;
;; Car av cdr g er hva 'adresse-feltet' (_Content of _Address _Register)
;; peker paa, og det er da dette paret (3 4).
;;
;; Address-reg i denne noden, car, peker videre til tallet 3.
;; Slik:
(define g '((1 2) (3 4)))
(car (car (cdr g)))
;; eller forkortet
(caar (cdr g))
;; eller base-case
(caadr g)
;; (car (car (cdr ((1 2) (3 4))))) 
;; (car (car ((3 4))
;; (car (3 4)
;; 3



;;----H -----------------------------------------------------------------------
;; Trekke ut tallet 3 fra ((1) (2) (3) (4)):
;;         a             b               c               d
;;     _car_cdr_     _car_cdr_       _car_cdr_       _car_cdr_
;;---->|_o_|_o-|---->|_o_|_o_|------>|_o_|_o_|------>|_o_|_\_|
;;       |             |               |               |
;;       |             |               |               |
;;       v             v               v               v
;;     _car_cdr_     _car_cdr_       _car_cdr_       _car_cdr_
;;     |_o_|_\_|     |_o_|_\_|       |_o_|_\_|       |_o_|_\_|
;;       |             |               |               |   
;;       |             |               |               |
;;       v             v               v               v
;;       1             2               3               4

;; Kort forklart, cdre to ganger slik at vi staar paa plass c, saa kikke paa
;; hva car til denne noden peker paa
(define h '((1) (2) (3) (4)))

;; Slik:
(car (cdr (cdr h))) ;; cdr h-->b-->c-->d, (cdr (cdr h))-->c-->d, car c-->3
;; Slik:
(car (cddr h))
;; Eller slik:
(caddr h)



;;----I --------------------------------------------------------------------
;; lage h med cons:
;;

(cons(cons 1 '())
	(cons (cons 2 '())
	      (cons (cons 3 '()) (cons (cons 4 '())'()))))

;; Dette blir egentlig en baklengs car/cdr-tankegang, (cons (car) (cdr)) 
;; skal proeve aa beskrive:
;;
;; - Foerst lager jeg en node hvor car peker til 4, cdr til nil. (cons 4 '()).
;; - Saa konses denne med en ny node hvor car peker til noden m/4 og cdr til nil.
;;   Og dette blir da slutten paa 'hoved/ytterste-liste'
;; - Saa konses dette igjen med en node hvor car -> (cons 3 '()) og
;;   cdr->til det beskrevet over.
;; - Saa gjentar jeg det med en ny node som bindes til alt det foregaaende.
;; - Og saa binder jeg alt dette til en ny node sin cdr, hvor car-> node m/car->2
;; - Til slutt bindes hele denne  med en node hvor car peker til en node med 1
;;  Altsaa er cdr til listen bygget opp baklengs.
;;
;;
;; lage h med list:
;; Denne var en del enklere, da den ligner paa selve definisjonen av
;; tegningen over av samme problem, hvor det beskriver en liste av lister.

(list (list 1) (list 2) (list 3) (list 4))




;;----------------------------- Oppgave 2 ------------------------------------


;;----A  --------------------------------------------------------------------
;; Halerekursiv prosedyre for aa finne lengden av en liste.

(define (length2 items)
  (define (length-iter length items)
    (if (null? items)
	length
	(length-iter (+ 1 length) (cdr items))))
  (length-iter 0 items))

(define a '(1 2 3 3 6 7))
(length2 a) ;; => 6

;; Her bruker jeg en indre halerekursiv iterator-prosedyre 
;; som inkrementerer 'length' med 1 ved hver iterasjon.
;; Antall iterasjoner bestemmes av listens lengde. Altsaa i det
;; predikatet null?, den tomme listen naaes.
;; Da vil prosedyren returnere length til ytre prosedyr som i sin
;; tur evalueres til den samme 'length'.



;;----B  --------------------------------------------------------------------
;; Prosedyre som reverserer listen.
;; Denne loesningnen  (hale-rekursiv) valgte jeg av bekvemlighet,
;; da det var den loesningen jeg klarte aa faa hodet mitt rund.
;; 
;; Prosedyren er iterativ da det ikke er noen ventende kall under
;; naavaerende kall paa stacken. Det er kun rev-list som venter
;; paa resultatet til rev-list-cons og ikke rev-list-cons som venter paa
;; neste resultat av seg selv.

;; Forklaringen paa hvordan jeg har tenkt er beskrevet under.
(define setning '("towel" "a" "bring" "always"))

(define (rev-list items)
  (define (rev-list-cons lst last)
    (if (null? lst)
	last
	(rev-list-cons (cdr lst) (cons (car lst) last))))
  (rev-list-cons items '()))

;; Bygger opp listen baklengs ved aa konse car (foerste element) med nil og
;; saa rekursivt konse foerste element i resten av listen med den forrige
;; noden.
;; Eksempel (rev-list '(1 2 3):
;; 1) in != null -> (r-l-c (2 3) (1 '()))
;; 2) in != null -> (r-l-c (3) (2 (1 '())))
;; 3) in != null -> (r-l-c ('()) (3 (2 (1 '()))))
;; 4) in == null -> ret: (3 2 1)
(rev-list setning)


;; Klarte ikke aa legge ifra meg ideen om at jeg skulle implementere
;; en rekursiv prosedyre for problemet, saa her er et forsoek.
;; Her er ideen aa loepe ut listen og for hvert kall skyve car ut og en plass
;; til hoyre. Visuelt ser jeg for meg at jeg tar cdr av listen og kiler denne
;; inn i front. Helt til det ikke er mer liste igjen. Da settes alt sammen
;; etter rekkefoelgen de fremstaar for append.
;; **(cons (car items) '()) er det samme som (list (car items)), men det er
;; lettere for meg aa foelge tankemoensteret ved aa bruke (cons).

(define (revlist items)
  (if (null? items)
      '()
      (append (revlist(cdr items)) (cons (car items) '()))))

;; (revlist '(1 2 3))
;; 1. (app (revlist '(2 3) (cons 1 '())))
;; 2. (append (append (revlist '(3)) (cons 2 '()) (cons 1 '())))
;; 3. (append ((append (r-l '())) (cons 3 '())) (cons 2 '()) (cons 1 '()))
;; 4. (append ('() (cons 3 '()) (cons 2 '()) (cons 1 '())))  
(revlist setning)



;;----C  --------------------------------------------------------------------
;;
;; Fjerne spesifikke elementer fra liste.

;; Dette blir en rekursiv prosedyre hvor 'else' venter paa aa konse
;; resultatet. Cons bygger seg opp rekursivt basert paa resultatet av
;; evalueringen av det resterende i lista. Faller saa sammen paa base-case '()
;; og bygger saa opp en liste av de ventende parene.

(define (ditch what lst)
  (cond ((null? lst) '())
	((equal? what (car lst)) (ditch what (cdr lst)))
	(else (cons (car lst) (ditch what (cdr lst))))))

(define e '(1 2 3 3 4 5 3))
(ditch 3 e)

;; Tanketre/Eksempel:
;;
;;    (null?) (3?)
;;      |       |------> kall prosedyren med resten (cdr).
;;      |
;;      |-------> returner null.
;;
;; Ellers: konse 1. element med resultat av overnevnte evaluering. 
;; Substitusjons-modell:
;; (ditch 3 '(1 2 3 4))
;; 1. ikke{null,3} -> (cons 1 (ditch 3 '(2 3 3 4))))
;; 2. ikke{null,3} -> (cons 1 (cons 2 (ditch 3 '(3 4)))))
;; 3. ikke null, men 3 -> (ditch 3 '(4))
;; 4. ikke{null,3} -> (cons 1 (cons 2 (cons 4 (ditch 3 '()))))
;; 5. null, <- base case
;; 6. (cons 1 (cons 2 (cons 4 '())))
;; 7. (cons 1 (cons 2 '(4 '()))))
;; 8. (cons 1 '(2 4))
;; 9. '(1 2 4)
;;
;; Angaaende stack-bruk til denne implementasjoen vil en ny stackframe
;; legges paa stacken frem til base case naas for hvert kall paa prosedyren.
;; Saa utloeses dominoeffekten paa de ventende rammene som kalkulerer sin retur-
;; verdi basert paa mottatt returverdi fra rammen over.
;; Til slutt returneres resultatet til kaller av prosedyren.
;;
;; Denne rekursive prosessen vokser proposjonalt med antallet elementer i
;; argumentet til prosedyren. I dette tilfellet lengden av listen.
;; Altsaa det er fire kall 'paa seg selv' frem til base-case i eksempelet.



;;----D  --------------------------------------------------------------------
;;
;; Fjerne element #index fra en liste.
;;
;; Her har jeg valgt en ganske simpel loesning. Den baserer seg
;; paa halerekusjon og test om elementets plassering er lik det som
;; oenskes fjernet. Saa lenge oensket posisjon ikke er naadd inkrementeres
;; telleren. Naar naad returneres elementet paa denne plassen.
;;
;; Det er som oppgaven beskriver ikke tatt hensyn til 'feil-bruk'.

(define (nth indx lst)
  (define (nth-iter cnt lst)
    (if (= cnt indx)
	(car lst)
	(nth-iter (+ cnt 1) (cdr lst))))
  (nth-iter 0 lst))

(nth 2 '(47 11 12 13))



;;----E --------------------------------------------------------------------
;;
;; Tankegangen paa loesningen paa dette problemet baserer seg i stor grad
;; paa prosessen over.
;; Igjen brukes en hale-rekursiv prosedyre, med en teller.
;; Saa lenge listen ikke er den tomme listen og elementet i naavaerende
;; posisjon ikke er lik oensket element, inkremerenteres telleren.
;;
;; Loeper listen ut, altsaa ingen match paa oensket element og faktisk element,
;; returneres false/#f.
;;
;; Hvis elem-argumentet matcher naavaerende element (car) returneres verdien
;; som ligger i telleren/cnt.

(define (where elem lst)
  (define (where-iter cnt lst)
    (cond ((equal? lst '()) #f)
	  ((equal? elem (car lst)) cnt)
	  (else (where-iter (+ 1 cnt) (cdr lst)))))
  (where-iter 0 lst))

(where 5 '(1 2 3 3 4 5 3)) ;;=> #5
(where 0 '(1 2 3 3 4 5 3)) ;;=> #f



;;----F --------------------------------------------------------------------

(define (map2 proc lst1 lst2)
  (if (or (null? lst1) (null? lst2))
      '()
      (cons (proc (car lst1) (car lst2))
	    (map2 proc (cdr lst1) (cdr lst2)))))

;; map2 tar in en prosedyre, og to lister som argument.
;; tester om den ene eller den andre listen er null og hvis saa
;; returnerer den tomme listen.
;; Hvis ikke:
;; konse resultatet av aa prosedere foerse element i den ene listen
;; med det foerste elementet i den andre listen.

;; Substitusjonsmodell:
;; (map2 + '(1 2 3 4) '(3 4 5))
;; (cons (+ 1 3) (map2 + '(2 3 4) '(4 5)))
;; (cons 4 (cons (+ 2 4) (map2 + '(3 4) (5))))
;; (cons 4 (cons 6 (cons (+ 3 5) (map + '(4) '()))))
;; (cons 4 (cons 6 (cons 8 '())))
;; (cons 4 (cons 6 8)
;; (cons 4 '(6 8)
;; '(4 6 8)

(map2 + '(1 2 3 4) '(3 4 5))



;;----G  --------------------------------------------------------------------
;; I denne oppgaven skal det lages en anonym prosedyre som regner ut
;; gjennomsnittet av to tall.
;; En anonym prosedyre er en prosedyre som ikke
;; ikke har et navn bundet til seg og foelgelig ikke kalles annet enn
;; 'in situ'.
;;
;; Det aa lage en anonym prosedyre for beregning av gjennomsnitt ville
;; antagelig ikke vaere hensiktsmessig for stoerre programmer. Dette fordi
;; det vil kunne vise seg aa vaere nyttig a ha denne prosedyren for haand.
;; Da ville man kanskje ha implementert den slik:

(define average
  (lambda (x y)
    (/ (+ x y) 2)))

;;eller slik:
(define (average x y)
  (/ (+ x y)))

;; Disse er da ikke anonyme prosedyrer, siden de er bundet til 'average'.
;; (Ser for meg at average er noekkelen for aa faa laget prosedyren definert
;; av lambda-prosedyren, ikke prosedyren selv.) 

;; Men trenger man kun aa faa utfoert en prosedyre som et ledd i en
;; stoerre sammensatt prosedyre slik som oppgaven spoerr om,
;; vil det vaere hensiktsmessig aa plugge oensket prosedyre
;; rett inn i den sammensatte prosedyren. Slik:

(map2 (lambda (x y)
	(/ (+ x y) 2))
      '(1 2 3 4) '(3 4 5)) ;;=> (2 3 4)


;; Viser at en anonym prosedyre kan kan brukes til aa faa en liste
;; av true/false-verdier der begge elementer er partall, ellers false:

(map2 (lambda (x y)
	(and (even? y) (even? x)))
      '(1 2 3 4 8 1 4) '(3 4 5 1 1)) ;; => (#f #t #f #f #f)


;; Her har jeg valgt aa bruke special-formen 'and', for aa kunne evaluere
;; begge resultatene samtidig. Dog lurker det en antagelse om at det
;; finnes en mer smidig loesning paa problemet..



;;----H --------------------------------------------------------------------
;;
;; Har jeg i bunn og grunn brukt samme prosedyre som over, de-anonymisert
;; den og gitt den et predikat som ekstra argument til prosedyren.
;; Siden det er en lambda-funksjon lages en ny prosedyre som tar inn
;; ytteligere 2 argumenter (x og y). Slik jeg ser det for meg blir
;; pred 'plugget' inn i prosedyren og 'x' og 'y' staar igjen som argumenter
;; til den nye prosedyren.

(define (both? pred)
  (lambda (x y)
    (and (pred y) (pred x))))

(map2 (both? even?) '(1 2 3) '(3 4 5))



;;----I --------------------------------------------------------------------
;;
;; Argumentet til self er en prosedyre og med det reevalueres argumentet til
;; self og self-selv til en ny prosedyre som tar ett argument x.

(define (self proc)
  (lambda (x)
    (proc x x)))

((self list) "hello")  ;;=> ("hello" "hello")
((self +) 5) ;;=> 10
((self *) 3) ;;=> 9
(self +)     ;;=> #<procedure>

