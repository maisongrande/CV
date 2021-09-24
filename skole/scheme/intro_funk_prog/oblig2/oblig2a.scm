(require r5rs)
(require racket/trace)
(load "huffman.scm")

;;---------------------------------------------------------------------------
;;
;; Obligatorisk oppgave 2b
;; Christopher Storheim.
;; Inf2810 Funkjsonell Programmering.
;; Beklager manglende norske vokaler!
;;
;;---------------------------------------------------------------------------


;;----------------------------------------------------------------------------
;;**************************** OPPGAVE 1 *************************************
;;----------------------------------------------------------------------------


;;----A ----------------------------------------------------------------------

;; En prosedyre p-cons som tar to argumenter x og y, som returnerer en
;; prosedyre som tar ett argument:
(define (p-cons x y)
  (lambda (proc) (proc x y)))

;; prosedyre p-car/cdr som tar ett argument og en prosedyre proc. Returnerer
;; resultatet av aa substituere argumentet til proc med kroppen til p-car.
(define (p-car proc)
  (proc (lambda (x y) x)))

(define (p-cdr proc)
  (proc (lambda (x y) y)))

;; Virker fordi:
;; Gitt at (p-car (p-cons 35 47)
;; --> (p-car (lambda (proc) (proc 35 47)))
;; Substituerer med lambda-prosedyren p-cons i p-car
;; --> ((lambda (proc) (proc 35 47)) (lambda (x y) x))
;; Substituerer proc med kroppen til p-car prosedyren.
;; --> ((lambda (x y) x) 35 47)
;; --> ((lambda (35 47) x)
;; Returnerer: 35, da tallet staar i x-posisjon.

(p-cons "foo" "bar") ;;=> #<procedure>
(p-car (p-cons "foo" "bar")) ;;=> "foo"
(p-cdr (p-cons "foo" "bar")) ;;=> "bar"
(p-car (p-cdr (p-cons "zoo" (p-cons "foo" "bar")))) ;;=> "foo"
;;-------------------------------------------------------------------



;;----B ----------------------------------------------------------------------

(define foo 30)

(let ((x foo)
      (y 20))
  (+ foo x y))

;; Svar, syntaktisk sukker for:
((lambda (x y)
   (+ x x y)) foo 20)
;;===================

(let ((foo 10)) ;;<- <var> <exp>
  (let ((x foo) ;;<- <body> som igjen er et let-uttryk avhengig
	(y 20)) ;; av forrige binding av foo.
    (+ foo x y)))

;; Omskrevet:

(let* ((foo 10)
       (x foo)
       (y 20))
  (+ foo x y))

;; Svar:
((lambda (foo)
   ((lambda (x)
      ((lambda (y)
	 (+ foo x y))
       20))
    foo))
 10)
;;===================
;; Foerst substitueres y med 20 i det innerste uttrykket.
;; Saa foo for x i det "midterste" uttrykket.
;; Naa ser det slik ut:
;; ((lambda (foo) (+ foo foo 20)) 10)
;; (Naa substitueres foo med 10 og uttrykket evalueres til 40





;;----C ----------------------------------------------------------------------
(define a1 (list 1 2 3 4))
(define a2 (list + - * /))
(define a3 (list 5 6 7 8))
;; Saa:
(map (lambda (x y z) (y x z)) a1 a2 a3)

;; Svar:
;; Prosedyren map som medfoelger Scheme tar variabelt antall argumenter,
;; saa lenge proc-argumentet har like mange variabler som antallet vedlagte
;; lister. Prosedyren utfoeres foerst paa 1. element i alle lister, saa
;; 2. element i alle lister osv til listene er loepet ut.
;; I oppgaven: (cons (+ 1 5) (cons (- 2 6) (cons (* 3 7) (cons (/ 4 8) '()))))
;;
;; -> (6 -4 21 1/2)
;;=================

;; Direkte kall (Litt usikker paa om jeg forstaar sproersmaalet):
((lambda (x y z)
   (y x z)) 3 * 7)
;; Resultat = 21
;; fordi ved substitusjon: (lambda (3 * 7) (y x z)) -> (* 3 7) = 21
;;                             ^ ^ ^
;;                             | | |
;;                             x y z




;;----------------------------------------------------------------------------
;;*************************** OPPGAVE 2 *************************************
;;----------------------------------------------------------------------------




;;----A ----------------------------------------------------------------------
;; Hjelpeprosedyre:
(define (member? what items)
  (cond ((null? items) #f)
	((eq? what (car items)) #t)
	(else (member? what (cdr items)))))

(member? 'forest '(lake river ocean)) ;;=> #f
(member? 'river '(lake river ocean)) ;;=> #t




;;----B ----------------------------------------------------------------------
;; Vitsen med den interne prosedyren er aa beholde en referanse til roten av
;; treet. Dette fordi at hver gang en blad-node blir funnet, blir symbolet til
;; denne konset sammen med resten av resultatet av aa foelge
;; de resterende bitene fra roten av treet.




;;----C ----------------------------------------------------------------------
;; Lag en halerekursiv versjon av prosedyren decode.

;; Hjelpeprosedyre
(define (rev-list lst)
  (define cons-up
    (lambda (lst last)
      (if (null? lst)
	  last
	  (cons-up (cdr lst) (cons (car lst) last)))))
  (cons-up lst '()))


(define (decode bits tree)
  (define (decode-1 result bits current-branch)
    (if (null? bits)
	result
	(let ((next-branch
	       (choose-branch (car bits) current-branch)))
	  (if (leaf? next-branch)
	      (decode-1 (cons (symbol-leaf next-branch)
			      result)
			(cdr bits) tree)
	      (decode-1 result (cdr bits) next-branch)))))
  (rev-list (decode-1 '() bits tree)))

;; For aa faa den halerekursiv har jeg valgt aa legge ved en tom liste
;; som resultatet av treff paa leaf-nodes blir konset til. Det eneste
;; som vokser da er lengden paa listen og hvis jeg ikke tar feil, er det da
;; stoerrelsen av aa allokere en ny node paa heapen som oeker per nytt treff.
;;
;; Ved aa gjoere det slik vil naturligvis meldingen reverseres, da foerste ord
;; konses med den tomme listen osv.





;;----D ----------------------------------------------------------------------
;; Resultatet av aa kalle prosedyren er:
(decode sample-code sample-tree)
;;=> (ninjas fight ninjas by night)




;;----E ----------------------------------------------------------------------
;; prosedyren faar inn f.eks: '(ninjas fight ninjas) og et tre.
;; Ser ca slik ut:
;;
;;
;;              . ((ninjas fight night by) 15)
;;             / \
;;            /   \
;;           /     \
;;          /       \
;;         /         \
;;      0 /           \ 1
;;       /             \
;;      /               O: ((fight night by) 7)
;;     /               / \
;; (leaf ninjas 8)    /   \
;;                 0 /     \ 1
;;                  /       \
;;         (leaf fight 5)    O: ((night by) 2)
;;                          / \
;;                         /   \
;;                      0 /     \ 1
;;                       /       \
;;                (leaf night 1)  \
;;                                 \
;;                             (leaf by 1)
;;
;; Lener meg paa oppgave 2.68 i sicp fordi den var en god maate aa komme
;; igang paa og stiller samme spoersmaal.
;;
;; I koden fra boken konses hvert resultat av prosedyrekallet til encode-
;; symbol til resultatet av aa kalle seg selv med neste element i symbollisten
;; og treet (sample-tree).

(define (encode symbol tree)
  (define encode-symbol
    (lambda  (symbol tree)
      (cond ((leaf? tree) '())
	    ((member? symbol (symbols (left-branch tree)))
	     (cons 0 (encode-symbol symbol (left-branch tree))))
	    ((member? symbol (symbols (right-branch tree)))
	     (cons 1 (encode-symbol symbol (right-branch tree)))))))
  (if (null? symbol)
      symbol
      (append (encode-symbol (car symbol) tree)
	      (encode (cdr symbol) tree))))

;; Hjelpeprosedyren left/right-branch returnerer listen over tre-strukturen
;; ved en node.
;; Hjelpeprosedyren symbols returnerer en liste over hvilke symboler
;; hver gren-node inneholder.
;;
;; (et eksempel):
;; kaller encode-symbol med fight.
;; Sjekker om fight er i venstre gren ved hjelp av member av symbols; nei.
;; Hoyre? Ja!
;; konser 1 med resultatet av aa gjenta samme algoritme ved neste tre-node
;; paa hoyre-gren.
;; sjekker igjen om left/right.
;; Denne gangen left, konser 0 med nytt kall. Naa evalueres treet til 'leaf
;; og kons-kjeden (0 1) lukkes. Encode, kaller seg selv og tester om det er
;; flere symboler i listen:

(encode '(ninjas fight ninjas) sample-tree) ;;=> (0 1 0 0)
(decode (encode '(ninjas fight ninjas) sample-tree) sample-tree)
;;=> (ninjas fight ninjas)




;;----F ----------------------------------------------------------------------
;; Tenkt oppfoersel, innkapsling av prosedyren:
;;               _____________
;;               |           |
;; freq-pairs -->| Black Box |---> tree
;;               |___________|
;;
;;
;; Foelger oppskriften gitt i boken.
;; Foerst sorteres parrene ved aa kalle paa make-leaf-set:
;; => {{leaf e 1} {leaf c 1} {leaf a 2} {leaf f 3} {leaf d 3} {leaf b 5}}
;;
;; Saa maa disse bindes sammen etter weight, det tar huffman.scm seg av
;; ved hjelp av prosedyren (make-code-tree). Denne prosedyren konser to noder
;; sammen med resultatet av aa konse symbolene til hver av nodene som igjen
;; konses med resultatet av aa summere vektene.
;; eks: => ((leaf e 1) (leaf c 1) (e c) 2)
;;
;; Naar dette er gjort maa denne noden bindes med resultatet av aa rekursivt
;; lage nye node-par. Dette haandteres av prosedyren adjoin-set.
;;
;; Base-case er naar vi er kommet til roten og vi har et node-par hvor
;; car peker paa treet selv og cdr er den tomme listen.
;; Dette er et resultat av  at adjoin-set treffer sitt
;; base case (cddr er '()). Da returneres car av roten tilbake til kaller.
;;
;;

(define (grow-huffman-tree freq-pairs)
  (define (join-branches sorted-freq-pairs)
    (if (null? (cdr sorted-freq-pairs))
	(car sorted-freq-pairs)
	(join-branches
	 (adjoin-set
	  (make-code-tree (car sorted-freq-pairs)
			  (cadr sorted-freq-pairs))
	  (cddr sorted-freq-pairs)))))
  (join-branches (make-leaf-set freq-pairs)))


(define freqs '((a 2) (b 5) (c 1) (d 3) (e 1) (f 3)))
(define codebook (grow-huffman-tree freqs))

(decode (encode '(a b c) codebook) codebook) ;; => (a b c)
;;========================================================



;;----G ----------------------------------------------------------------------

(define alfabet '((ninjas 57) (samurais 20) (fight 45) (night 12) (hide 3)
		  (in 2) (ambush 2) (defeat 1) (the 5) (sword 4) (by 12)
		  (assassin 1) (river 2) (wait 1) (poison 1)))

(define gTree (grow-huffman-tree alfabet))

(define sentence '(ninjas fight
			  ninjas fight ninjas
			  ninjas fight samurais
			  samurais fight
			  samurais fight ninjas
			  ninjas fight by night))

(define bit-sequence (encode sentence gTree))

;; 1) Hvor mange bits bruker det paa aa kode meldingen?
(define (count-elements items)
  (define (iter i items)
    (if (null? items)
	i
	(iter (+ 1 i) (cdr items))))
  (iter 0 items))

(count-elements bit-sequence)
;;= 40
;;========

;; 2 Gjennomsnittlige lengde paa hvert kodeord:
(define avrg-code-word-len
  (/ (count-elements bit-sequence) (count-elements sentence)))
avrg-code-word-len ;; = 40/17 = ~2.35 bits
;;                    ====================

;; Minste antallet man vil trenge (ASCII) er 8bit/char multipliert med antal
;; char i strengen. Sett bort i fra space-typer vil det si*:

;; 6*ninjas = 6*(6char*8bit)     = 288
;; + 6*fight = 6*(5char*8bit)    = 240
;; + 3*samurais = 3*(8char*8bit) = 192
;; + 1*by = 1*(2char*8bit)       =  16
;; + 1*night = 1*(5char*8bit)    =  40
;; -----------------------------------
;; Totalt antall bit for setning = 776
;; ===================================

;; * Forutsetter at disse skal kodes paa en datamaskin.
;; Gaar antallet char i strengen opp i  8 vil det si at minimum
;; blir antall char i strengen - 1.
;; Gjoer man det for haand etter asciitabellen vil hver char oppta 7 bit og alle
;; steder det staar 8 over kan byttes ut med 7.




;;----H ----------------------------------------------------------------------
;; Prosedyren huffman-leaves
;; ---huffman-tre-->PROSEDYRE--(par av symboler)

(define (huffman-leaves tree)
  ;; Lagre medlemmene i treet
  (define member-names (symbols tree))
  (traverse tree member-names))

;; Hjelpeprosedyre
(define (traverse tree symbol)
  (define (find-leaf-node symbol tree)
    (cond ((leaf? tree)
	   (cons (symbol-leaf tree) (weight tree)))
	  ((member? symbol (symbols (left-branch tree)))
	   (find-leaf-node symbol (left-branch tree)))
	  ((member? symbol (symbols (right-branch tree)))
	   (find-leaf-node symbol (right-branch tree)))))
  (if (null? symbol)
      symbol
      (cons (find-leaf-node (car symbol) tree)
	    (traverse tree (cdr symbol)))))


(huffman-leaves sample-tree)
;;=> {{ninjas . 8} {fight . 5} {night . 1} {by . 1}}
;;===================================================




;;----I ----------------------------------------------------------------------
;; p(Si) = symbolets vekt / treets samlede vekt.
;; |Ci|  = antallet bit som HT bruker paa aa kode symbolet.

;; p(si): Her har jeg vekten til symbolet i (huffman-leaves tree)
;;        og jeg har treets samlede vekt i (weights tree)
;;
;; |Ci| : Hvis jeg sender inn et symbol inn i (decode) faar jeg veien
;;        gjennom treet til denne noden og hvis jeg saa summerer antallet
;;        bits faar jeg oensket antall.

;; Loesning:

;;Hjelpeprosedyrer:
(define abs-value
  (lambda (x)
    ((if (> 0 x) - +) 0 x)))

(define total-freq
  (lambda (tree)
    (weight tree)))

(define relativ-freq
  (lambda (code-pair)
    (cdr code-pair)))

;; Del-funksjoner i det matematiske utrykket:
(define (abs-Ci code-word tree)
  (abs-value
   (count-elements
    (encode (list (car code-word)) tree))))

(define probability-Sc
  (lambda (code-word tree)
    (/ (relativ-freq code-word)
       (total-freq tree))))

(define (partial-sum tree code-word)
  (* (probability-Sc code-word tree)
     (abs-Ci code-word tree)))

;; Expected Code Length:
(define (expected-code-length tree)
  (define (sum-up code-word)
    (if (null? code-word)
	0
	(+ (partial-sum tree (car code-word))
	   (sum-up (cdr code-word)))))
  (sum-up (huffman-leaves tree)))

;; Svar:
(expected-code-length sample-tree) ;;=> 8/5
;;=========================================
