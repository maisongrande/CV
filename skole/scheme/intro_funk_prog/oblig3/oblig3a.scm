;;#############################################################################
;;
;;                          Oblig 3a Inf2810
;;                        Christopher N .Storheim
;;
;;                  -- Beklager manglende norske vokaler --
;;
;;#############################################################################

(require r5rs)
(load "prekode3a.scm")
(require racket/trace)



;;;Oppgave 1
;; Denne fikk jeg ikke til annet enn oppgave a. Jeg klarer ikke helt se
;; for meg hvordan jeg skal endre bindingen til prosedyre-argumentet
;; tilbake til seg selv.
;; Slik jeg har tenkt er aa lagre prosedyre-objektet i en tabel kalt
;; proc-list ved kall til memoize, for slik a kunne faa returnert denne
;; naar unmemoize blir kalt, og paa den matten gjennopprette bindingen.
;; Dette har ikke fungert, men jeg har latt forsoeket bestaa for aa
;; vise tanken.

;; a) og b)

(define (mem msg proc)
  ;; Utvidet og bygget fra sicp 3.27
  (let ((table (make-table))
	(proc-list (make-table)))
    (lambda args
      (cond ((equal? msg 'memoize)
	     ;; memoize
	     (let ((this-proc proc)
		   (prev-comp-res (lookup args table)))
	       (or prev-comp-res
		   (let ((result (apply proc args)))
		     (insert! args result table)
		     result)
		   (insert! this-proc proc proc-list)
		   this-proc)))
	    ;; unmemoize
	    ((equal? msg 'unmemoize)
	     (lookup proc proc-list))
	    (else "Unknown")))))


;; Test-prosedyrer:
(set! fib (mem 'memoize fib))
(fib 3)
(set! fib (mem 'unmemoize fib))
(fib 3) ;; Her blir fib satt til #f som indikerer at jeg ikke har faat
	;; cached fib i utgangspunktet. (#f returverdi fra lookup-proc)
(set! test-proc (mem 'memoize test-proc))
(test-proc 40 41 42 43 44)


;;-----------------------------------------------------------------------------



;;;Oppgave 1
;; c)


(define memfib (mem 'memoize fib))
(memfib 5)
(memfib 4)

;; Bindingen til prosedyren fib blir ikke endret, den har fortsatt binding til
;; det globale miljoet. Sa naar memfib kalles med argument vil fib fortsatt
;; rekursere med kall til den globale-fib (seg selv). Disse mellomresultatene
;; vil dermed ikke memoiseres (pga bindingen til det global-env), men det
;; endelige resultatet (retur verdien til fib) blir memoisert slik vi har
;; bedt om.


;;-----------------------------------------------------------------------------



;;;Oppgave 1
;; d)

(define (arg-look-up key args default)
  ;; Sjekker vha member om gitt symbol finnes
  ;; i argument listen. Hvis sa returnerer denne.
  ;; Ellers returneres verdien gitt av default.
  (let ((test (member key args)))
    (cond (test (string-append " " (cadr test)))
	  (else (string-append " " default)))))



(define greet
  ;; Denne sammen med oppg1 gjorde meg lettere-gal, saa implementasjonen
  ;; er ganske teit.. Ideen var jo selvfolgelig(?) aa faa til
  ;; en eller annen form for apply/map/loop/for-each over argumentene til
  ;; arg-look-up, men jeg ble ikke klok pa feilene som kom i retur.
  ;; Derfor denne 'generell for 2 keywords'-loesningen..
  (lambda args
    (let ((default-start "good")
	  (default-vals '("day" "friend"))
	  (key-words (list 'time 'title)))
      (string-append default-start
		     (arg-look-up (car key-words) args (car default-vals))
		     (arg-look-up (cadr key-words) args (cadr default-vals))))))


;; tester:
(greet) ;;=> "good day friend"
(greet 'time "evening") ;;=> "good evening friend"
(greet 'title "sir" 'time "morning") ;;=> "good morning sir"
(greet 'time "afternoon" 'title "dear") ;;=> "good afternoon dear"


;; Alternativ2 med bruk av display:

(define greet2
  ;; Denne viser tanken jeg beskrev over, men akkumulerer ingenting.
  ;; Dette er en mulig losning pa oppgaven siden oppgaven ikke
  ;; har ""-tegn i resultatet? I det minste er den mer generell...
  (lambda args
    (let ((default-start "good")
	  (default-vals '("day" "friend"))
	  (key-words (list 'time 'title)))
      (define (loop-over kw dv)
	(if (null? kw)
	    (newline)
	    (begin (display (arg-look-up (car kw) args (car dv)))
		   (loop-over (cdr kw) (cdr dv)))))
      (display default-start)
      (loop-over key-words default-vals))))

;; tester:
(greet2)
;;=> #<void>
;;good day friend
(greet2 'time "evening")
;; => #<void>
;;good evening friend
(greet2 'time "afternoon" 'title "dear")
;;=> #<void>
;;good afternoon dear
(greet2 'title "sir" 'time "morning")
;;=> #<void>
;;good morning sir


;;-----------------------------------------------------------------------------



;;;Oppgave 2
;; a)

;; List to stream:
(define (list-to-stream lst)
  ;; Konser car av listen med den forsinkede
  ;; evalueringen av resten av listen.
  (if (null? lst)
      the-empty-stream
      (cons-stream (car lst)
		   (list-to-stream (cdr lst)))))

;; Stream to list:
(define (stream-to-list s . args)
  ;; Sikkert en mye penere losning paa denne..
  ;; Enten er antallet (n) styrende for hvor
  ;; mange elem av streamen som skal listes,
  ;; ellers saa er den uvesentlig og divergerer fra
  ;; null.
  (define (iter s n)
    (if (or (stream-null? s)
	    (= 0 n))
	'()
	(cons (stream-car s)
	      (iter (stream-cdr s) (- n 1)))))
  (iter s (if (null? args) -1 (car args))))


;;; Testprosedyrer:
(list-to-stream '(1 2 3 4 5))
(stream-to-list (stream-interval 10 20))
(show-stream nats 15)
(stream-to-list nats 10)


;;-----------------------------------------------------------------------------



;;;Oppgave 2
;; b)


(define (contains-null? lst)
  ;; Sjekker rekursivt liste-element for
  ;; liste-element for den tomme listen
  ;; i argstreams.
  (cond ((null? lst) #f)
	((null? (car lst)) #t)
	(else (contains-null? (cdr lst)))))


(define (stream-map proc . argstreams)
  ;; Forslag til losning basert pa gitt kode:
  (if (contains-null? argstreams)
      the-empty-stream
      (cons-stream
       (apply proc (map (lambda (elem)
			  (stream-car elem)) argstreams))
       (apply stream-map
	      (cons proc (map (lambda (elem)
				(stream-cdr elem)) argstreams))))))

;; Test av uendelig + endelig + endelig strom:
(show-stream (stream-map + nats
			 (list-to-stream '(1 2 3 4 5))
			 (list-to-stream '(4 5 6 7))) 10)
;; => 6 9 12 15


;;-----------------------------------------------------------------------------



;;;Oppgave2
;; c)

;; Petter Smart sin losning vil gjoere jobben til memq
;; 'uendelig' vanskelig siden den er nodt til aa sjekke om den
;; potensielt 'uendelige' streamen inneholder et duplikat av
;; foerste element i lst. Dette kan ta 'uendelig' med tid hvis
;; listen er uendelig.


;;-----------------------------------------------------------------------------



;;;Oppgave2
;; d)

(define (remove-duplicates stream)
  ;; remove-duplicates lager en ny stream ved
  ;; a konse forste elem i streamen med resten av streamen.
  ;; Resten av streamen filtreres mot allerede tvungede
  ;; evalueringers tidligere resultater. Tanken er samme som
  ;; sieve for prime f.eks.
  (if (stream-null? stream)
      the-empty-stream
      (cons-stream (stream-car stream)
		   (remove-duplicates
		    (stream-filter
		     (lambda (elem)
		       (not (eq? elem (stream-car stream))))
		     stream)))))

(define test-remove
(list-to-stream '(1 2 3 3 4 5 3 6 7 8 2 9 10)))
;; Duplikater       ^ ^     ^ ^     ^
(show-stream (remove-duplicates test-remove) 20)
;;=> 1 2 3 4 5 6 7 8 9 10


;;-----------------------------------------------------------------------------



;;;Oppgave2
;; e)

(define (show x)
  (display x)
  (newline)
  x)

(define x
  (stream-map show
	      (stream-interval 0 10)))
;;=> 0
;; Her vises stream-car av stream-interval,
;; og den memoiseres.

(stream-ref x 5)
;;=> 5
;;1
;;2
;;3
;;4
;;5
;; 0 er memoisert saa den vises ikke, men suksessive
;; tall til og med x skrives og memoiseres.

(stream-ref x 7)
;;=> 7
;;6
;;7
;; Oppsummert: For aa unngaa repetitive evalueringer
;; memoiserer Scheme resultatet av a kalle force
;; pa det "ventende" objektet. Dvs stream-ref sitt
;; kall pa stream-cdr forer til memoizering av resultatet.


;;-----------------------------------------------------------------------------



;;;Oppgave2
;; f)

(define mul-streams
  ;; mul-stream mapper multiplikasjon
  ;; over stream-elementene.
  ;; Illustrert under factorials.
  (lambda (s1 s2)
    (stream-map * s1 s2)))

(define factorials (cons-stream 1 (mul-streams nats factorials)))

;; Tenkt slik:
;;
;;   1                        <-stream-car
;;                            stream-map:
;;        1   2   3   4   5   <-nats
;; *    .>1 .>1 .>2 .>6 .>24  <-factorials
;;=====/===/===/===/===/================
;;   1   1   2   6   24  120  <-factorials


(stream-ref factorials 5)
;;=> 120

;;-----------------------------------------------------------------------------
