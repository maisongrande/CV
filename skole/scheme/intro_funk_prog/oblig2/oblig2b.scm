(require r5rs)
(require racket/trace)
(load "oblig2b_tester.scm")
;;=============================================================================
;;
;; Obligatorisk Oppgave 2b
;; Christopher Storheim
;; Inf 2810
;;
;;=============================================================================

;;==Oppgave 1 =================================================================

;;==A

(define (make-counter)
  ;; Ved kall pa proc make-counter returneres prosedyre-obj.
  ;; definert av lambda-uttrykket, hvor den lokale
  ;; variablen 'count' initielt er satt til 0 vha let.
  ;; Ved hvert suksessive kall pa pros.obj., endres tilstanden
  ;; til objektet det pekes til ved aa oeke det med 1 vha set!.
  ;; Objektets verdi returneres til kaller ved hvert kall.
  (let ((count 0))
    (lambda ()
      (set! count (+ 1 count))
      count)))

(define count 42)
(define c1 (make-counter))
(define c2 (make-counter))

(c1) ;;=> 1
(c1) ;;=> 2
(c1) ;;=> 3
count;;=> 42
(c2) ;;=> 1
;;_____________________________________________________________________________


;;==B
;;			   +-----------------+
;; Global Environment:	   |	+---------+  |
;; +------------------+    |    |         V  |    Prosedyre-objektet
;; | *  '  car        |<---+    |        / \/o\   som er bundet
;; |                  |         |        \|/\ /   til navnet make-counter
;; |   cdr            |		|	  |  V    i det glob.env. etter
;; | + -   osv. osv.  |		|	  |       evaluering av lambda uttrykket
;; |                  |         |         V       gitt av define.
;; |                  |         |
;; |                  |         |        (let ((count 0))
;; |                  |         |           (lambda ()
;; |                  |         |          (set! count (+ 1 count))
;; |                  |         |          count))
;; |                  |         |
;; |                  |         |
;; |                  |		|
;; | make-counter =---+---------+  c1 sitt env. ramme
;; |                  |		      +----------+
;; |                  |<--------------|count: 3  |<------------+
;; |                  |               +----------+    +-----+  |
;; |                  |                               |     |  |
;; | c1  = -----------+-------------------------------+     |  |
;; |                  |                                     |  |
;; |                  | c2 sin env.ramme                    v  |
;; |                  |  +----------+                      / \/o\ Obj. som i det
;; |                  |<-|count: 1  |<--+                  \|/\ / g.env har navn
;; |                  |  +----------+   |                   |  V  c1.
;; |                  |                 |                   |
;; |                  |                 |                   v
;; | c2  = -----------+--------------+  |             (lambda ()
;; |                  |              V  |              (set! count (+ 1 count))
;; |                  |             / \/o\ Obj.c2      count)
;; | count: 42        |             \|/\ /
;; +------------------+		     |  V
;;				     |
;;				     v
;;				    (lambda ()
;;                                   (set! count (+ 1 count))
;;                                   count)
;;
;;
;; Variablen count fra (define count 42) er def/opprettet i det globale env.
;; og verdien finnes da i denne rammen.
;;
;; Naar c1 og c2 defineres, evalueres forst uttrykket make-counter i det globale
;; env. og det opprettes en lokal env.ramme for dem, inneholdene hver sin
;; lokale versjon av m-c-variabelen count satt til 0 ved let.
;; Disse rammene peker tilbake til det lokale env fra hvor de ble opprettet.
;;
;; Siden m-c er et lambda uttrykk opprettes det h.h.v to prosedyreobjekter
;; som peker paa hver sin lokale ramme de ble opprettet i, kroppen til m-c,
;; samt parameterene i kroppen (Dette glemte jeg a tegne inn).
;; Til slutt bindes navnene c1 og c2 til hver sin utgave av disse
;; prosedyre-objektene i G.E.
;;
;;==Oppgave 2 =================================================================

;;== A
(define (make-stack init-elem)
  ;; make-stacks formal-parameter er foerste element i stacken.
  ;; Naar make-stack kalles returneres et prosedyre-objekt
  ;; inneholdenene en ferdig initialisert stack hvor det skal kunnes
  ;; legges til og fjernes elementer suksessivt ved kall til
  ;; prosedyreobjektets interne prosedyrer via message passing.
  (let ((stack (if (list? init-elem)
		   init-elem
		   ;; Hvis ikke liste, saa gjoeres det om til liste.
		   ;; Dette fordi jeg vil bygge stakken ikke-par ogsaa.
		   (list init-elem))))
    (define (flip lst)
      ;; Prosedyren flip, reverserer argumentene til push
      ;; naar denne blir kalt. De vil vaere paa liste-form
      ;; pga bruken av 'dotted tail arguments'.
      (define (cons-up items base)
	(if (null? items)
	    base
	    (cons-up (cdr items) (cons (car items) base))))
      (cons-up lst '()))
    (define (push! head)
      ;; push! er en destruktiv prosedyre som tar inn et
      ;; vilkaarig antall elementer paa liste-form fra
      ;; message passeren og appender disse til stakken.
      (set! stack (append head stack)) "push ok")
    (define (pop!)
      ;; pop! er en destruktiv prosedyre som fjerner det sist
      ;; tillagte elementet fra stacken. Foerst sjekkes det om
      ;; stakken er tom og hvis saa returneres en tom liste (nop).
      ;; Ellers saa settes stack-"pekeren" til aa peke paa resten
      ;; av listen. Garbage collectoren fjerner det forlatte
      ;; parret som foer var starten av stacken.
      (if (null? stack)
	  '()
	  (begin
	    (let ((tail-of-stack (cdr stack)))
	      (set! stack tail-of-stack)) "pop ok!")))
    ;; prosedyreobjektet som returneres og som all kommunikasjon
    ;; foregaar igjennom. cmd er car, x er cdr av formal-param.
    (lambda (cmd . x)
      (cond ((eq? cmd 'push!)
	     (push! (flip x)))
	    ((eq? cmd 'pop!)
	     (pop!))
	    ((eq? cmd 'stack)
	     stack)
	    (else "Unknown command")))))


(define s1 (make-stack (list 'foo 'bar)))
(define s2 (make-stack '()))
(s1 'pop!)
(s1 'stack) ;;=> {bar}
(s2 'pop!)
(s2 'push! 1 2 3 4)
(s2 'stack) ;;=> {4 3 2 1}
(s1 'push! 'bah)
(s1 'push! 'zap 'zip 'baz)
(s1 'stack) ;;=> {baz zip zap bah bar}
;;_____________________________________________________________________________


;;== B
(define (push! proc-object elem . more)
  ;; push abstraherer bort message-passing.
  ;; Tester om det er ett eller flere argumenter.
  ;; Hvis ett, kaller prosedyreobjektet med dette.
  ;; Ellers pusher elemt for elemt fra listen over
  ;; tilleggsagumenter (more).
  (define (successive-push lst)
    (cond ((null? lst) '())
	  (else (proc-object 'push! (car lst))
		(successive-push (cdr lst)))))
  (cond ((null? more)
	 (proc-object 'push! elem))
	(else (proc-object 'push! elem)
	      (successive-push more))))

(define (pop! proc-object)
  ;; Pop! utfoerer et kall til den interne prosedyren
  ;; pop! i prosedyreobjektet via message-passing.
  (proc-object 'pop!))

(define (stack proc-object)
  ;; Stack virker slik pop! gjoer, med annen
  ;; message.
  (proc-object 'stack))

(pop! s1)
(stack s1) ;;=> {zip zap bah bar}
(push! s1 'foo 'faa)
(stack s1) ;;=> {faa foo zip zap bah bar}



;;==Oppgave 3 =================================================================
;;
;;==A:
;;
;; Initielt etter define:
;;      _car_cdr_     _car_cdr_   _car_cdr_   _car_cdr_   _car_cdr_
;;      |   |   |     |   |   |   |   |   |   |   |   |   |   |   |
;; bar->|_o_|_o-|---->|_o_|_o-|-->|_o_|_o-|-->|_o_|_o-|-->|_o_|_\_|
;;        |               |           |           |         |
;;        v               v           v           v         v
;;       'a              'b          'c          'd        'e
;;
;; Etter (set-car! (cdddr bar) (cdr bar)):
;;
;;
;;                 .------------------------------------.
;;                 |                                    |
;;      _car_cdr_  |  _car_cdr_   _car_cdr_   _car_cdr_ |  _car_cdr_
;;      |   |   |  |->|   |   |   |   |   |   |   |   | |  |   |   |
;; bar->|_o_|_o-|---->|_o_|_o-|-->|_o_|_o-|-->|_o_|_o-|.J  |_o_|_\_|
;;        |               |           |           |          |
;;        v               v           v           v          v
;;       'a              'b          'c          'd         'e
;;
(define (lst-ref items n)
  (if (= n 0)
      (car items)
      (lst-ref (cdr items) (- n 1))))
;; Foerst og fremst saa er list-ref en prosedyre som returnerer det
;; car-feltet til den nte cons-cellen i listen peker paa. Hvor n er
;; argumentet til list-ref.
;;
;; Prosedyren set-cdr! er en prosedyre som bruker den destruktive
;; special-formen set! til aa permanent endre hvilken adr. cdr-feltet
;; inneholder. I dette tilfellet settes cdr-feltet til (cdr (cdr (cdr bar)))
;; til aa peke paa (cdr bar) som er resten av listestrukturen etter 'a (car a).
(define bar (list 'a 'b 'c 'd 'e))
(set-cdr! (cdddr bar) (cdr bar))
;;_____________________________________________________________________________

;; B
;; Initielt etter define:
;;
;;      _car_cdr_   _car_cdr_   _car_cdr_
;;      |   |   |   |   |   |   |   |   |
;; bah->|_o_|_o-|-->|_o_|_o-|-->|_o_|_\_|
;;        |           |           |
;;        v           v           v
;;     'bring        'a        'towel
;;
;;
;; Etter foerste set-car:
;;
;;      _car_cdr_       _car_cdr_   _car_cdr_
;;      |   | o-|------>|   |   |   |   |   |
;; bah->|_o_|___|  +--->|_o_|_o-|-->|_o_|_\_|
;;        |        |      |           |
;;        |        |      v           v
;;        +--------+      'a        'towel
;;
;; Baade car og cdr av foerste cons-paret i bah deler samme liste-struktur
;; (adr. i minnet). Det leder til at en hver destruktiv operasjon
;; paa car av foerste par, leder til samme resultat for cdr og omvendt.
;; Derfor vil (set-car! bah 42) endre car av cdr som er identisk med car av bah.
;;
;; Digresjon:
;; Det skumle er at det er vanskelig aa se uten sjekk med predikatet eq?
;; som tester objekt-likhet.
;; bah ;;=> ((a towel) a towel) <- kunne ha vaert:
;; la x og y vaere: (list 'a 'towel))
;; (define z (cons x y))
;; z => (mcons (mcons 'a (mcons 'towel '())) (mcons 'a (mcons 'towel '())))
;; bah => (mcons (mcons 'a (mcons 'towel '())) (mcons 'a (mcons 'towel '())))
;; (eq? (car z) (cdr z)) => #f
;; (equal? (car z) (cdr z)) => #t
;;_____________________________________________________________________________

;;==C
(define (cycle? lst)
  (define (clock inner outer)
    ;; Clock fungerer som en liten klokke hvor minutt-viseren
    ;; (outer) tikker raskere relativt til timeviseren, med
    ;; dobling av avstanden inntil den tar timeviseren igjen. I safall
    ;; er 'klokken' sirkulaer. Hvis den aldri tar igjen og heller
    ;; moeter tom liste, er den lineaer.
    (cond ((or (null? outer)
	       (null? (cdr outer))) #f)
	  ((or (eq? inner outer)
	       (eq? inner (cdr outer))) #t)
	  (else (clock (cdr inner) (cddr outer)))))
  (if (null? lst)
      #f
      (clock lst (cdr lst))))

(cycle? '(hey ho))   ;;=> #f
(cycle? '(la la la)) ;;=> #f
;;(cycle? bah)         ;;=> #f
(cycle? bar)         ;;=> #t
;;_____________________________________________________________________________

;;==D
;; Disse er ikke lister da en liste er definert til aa vaere av endelig lengde
;; og avsluttet med den tomme listen.

;;_____________________________________________________________________________
;;==E
;; I prosedyren Make ring har jeg valgt aa pakke inn alt som skal til
;; for aa lage en ring og samhandle med det returnerte prosedyre-objektet.
;; Det er en del interne prosedyrer som finnes i r5rs, men for treningens
;; skyld er de laget og implementert. Derfor blir det en ganske stor
;; prosedyre.


(define (make-ring items)
  ;; Hjelpeprosedyrer:
  (define (copy-list items)
    ;; Kopierer, reverserer og returnerer en kopi av
    ;; listen som ble sendt inn som argument.
    (define (revlist items)
      (if (null? items)
	  items
	  (append (revlist(cdr items))
		  (cons (car items) '()))))
    (define (cons-up in-lst out-lst)
      (if (null? in-lst)
	  (revlist out-lst)
	  (cons-up (cdr in-lst)
		   (cons (car in-lst) out-lst))))
    (cons-up items '()))
  (define (len-list lst)
    ;; Returnerer lengden paa den ikke sirkulaere listen.
    (if (null? lst)
	0
	(+ 1 (len-list (cdr lst)))))
  (define (lst-ref items n)
    ;; Returnerer elementet paa plass n
    (if (<= n 0)
	(car items)
	(lst-ref (cdr items) (- n 1))))
  (define (last-node lst)
    ;; Returnerer det siste parret i en liste.
    (if (null? (cdr lst))
	lst
	(last-node (cdr lst))))
  ;; Initialbetingelser:
  (let* ((len (len-list items))
	 (lst (copy-list items))
	 (top-of-ring (car lst))
	 (globl-n len))
    ;; Lag "sirkulaer" liste hvor cdr-feltet til siste
    ;; element i listen faar adressen til listen selv:
    (set-cdr! (last-node lst) lst)
    ;; Prosedyreobjekt-metoder
    (define (left-rotate)
      ;; Flytter top-of-ring-pekeren ett par mot venstre.
      ;; og reset referansen hvis den overskrider ant elem.
      (set! top-of-ring (lst-ref lst (+ 1 globl-n)))
      (set! globl-n (if (= (+ globl-n 1) len)
			0
			(+ globl-n 1)))
      top-of-ring)
    (define (right-rotate)
      ;; Flytter top-of-ring-pekeren ett par mot hoyre,
      ;; passer paa referansen.
      (set! top-of-ring (lst-ref lst (- globl-n 1)))
      (set! globl-n (if (>= 0 (- globl-n 1))
			len
			(- globl-n 1)))
      top-of-ring)
    (define (insert! new-node)
      ;; Setter inn et nytt par i listen foer top,
      ;; ved aa dirigere pekere.
      (define (iter items)
	(cond ((eq? (cadr items) top-of-ring)
	       (set-cdr! new-node (cdr items))
	       (set-cdr! items new-node)
	       (set! len (+ len 1)) (car new-node))
	      (else (iter (cdr items)))))
      (iter lst))
    (define (delete)
      ;; Fjerner elementet som top peker pa fra listen.
      ;; ved aa omdirigere peker rundt elem som skal bort.
      (define (iter items)
	(cond ((eq? (cadr items) top-of-ring)
	       (set-cdr! items (cddr items))
	       (set! top-of-ring (cadr items))
	       (set! len (- len 1))
	       top-of-ring)
	      (else (iter (cdr items)))))
      (iter lst))
    ;; Prosedyreobjektet som returneres ved kall:
    (lambda (msg . x)
      (cond ((eq? msg 'left-rotate) (left-rotate))
	    ((eq? msg 'top) top-of-ring)
	    ((eq? msg 'right-rotate) (right-rotate))
	    ((eq? msg 'insert) (insert! x))
	    ((eq? msg 'delete!) (delete))
	    (else "Unknown command")))))


;; Grensesnittet mot make-ring objektet:
(define (top proc)
  (proc 'top))
(define (left-rotate! proc)
  (proc 'left-rotate))
(define (right-rotate! proc)
  (proc 'right-rotate))
(define (delete! proc)
  (proc 'delete!))
(define (insert! proc elem)
  (proc 'insert elem))
;; Tester som beskrevet:
(define r1 (make-ring '(1 2 3 4)))
(define r2 (make-ring '(a b c d)))
(top r1) ;; => 1
(top r2) ;; => a
(right-rotate! r1) ;; => 4
(left-rotate! r1)  ;; => 1
(left-rotate! r1)  ;; => 2
(delete! r1)       ;; => 3
(left-rotate! r1)  ;; => 4
(left-rotate! r1)  ;; => 1
(left-rotate! r1)  ;; => 3
(insert! r2 'x)    ;; => x
(right-rotate! r2) ;; => d
(left-rotate! r2)  ;; => x
(left-rotate! r2)  ;; => a
(top r1) ;; => 3

;;_____________________________________________________________________________
;; Kompleksitet for baade left/right-rotate:
;; (Disclaimer: Dette kan jeg veldig lite om saa jeg proever meg frem etter
;; litt lesing paa nettet.)
;; -lst-ref loeper gjennom n +/-1 ganger, hvor n er 'omkretsen' av listen
;;   list-ref har 2 operasjoner en aritmetisk og et predikat.
;;   Snittet blir da 2*n for dette kallet.
;;
;; - Saa er det en set! operasjon som antatt tar 1 steg
;;
;; - Saa er det et predikat som tester en aritm.op, +2 steg
;;
;; - Avhengig av resultatet av predikatet kan det forekomme en aritm. op.
;;   worst case er da +1.
;;
;; - +1 for ny set!
;;
;; Totalt for aa flytte pekeren ett steg tar da:
;; 2*n + 5. Regner med at for store n vil 5 bli uvesentlig og for veldig store
;; lister vil faktorens betydning krympe. Dermed er kompleksiteten
;; = O(n). Altsaa tiden det tar aa flytte ett steg vokser lineaert
;; med stoerrelsen paa input-listen.
