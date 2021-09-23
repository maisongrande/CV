(require r5rs)


;;===================================
;;; Mengdekonstruktoerer:
;;===================================

(define (range from to step)
  ;; Lager en liste fra og med parameterverdi 'from'
  ;; til parameterverdien 'to' i steg pa 'step'.
  (if (= to from)
      '()
      (cons from
	    (range (+ step from) to step))))



(define (make-grid n m lst)
  (define (row-maker seq cnt)
    (if (= cnt 0)
	'()
	(cons (car seq)
	      (row-maker (cdr seq) (- cnt 1)))))
  (if (null? lst)
      '()
      (cons (row-maker lst n)
	    (make-grid n m (slice m (length lst) lst)))))


;;===================================	
;;;; Grunnleggende prosedyrer:
;;===================================

(define (filter pred seq)
  ;; Filtrer ut verdier av sekvensen 'seq' som evaluerer
  ;; til sant i predikatet gitt av 'pred'.
  ;; Returnerer en ny allokert liste av disse verdiene.
  (cond ((null? seq) '())
	((pred (car seq))
	 (cons (car seq)
	       (filter pred (cdr seq))))
	(else (filter pred (cdr seq)))))




(define (map proc seq)
  ;; Utforer prosedyren git av proc pa hvert
  ;; element i listen 'seq'. Returnerer en ny
  ;; allokert liste av disse verdiene.
  (if (null? seq)
      '()
      (cons (proc (car seq))
	    (map proc (cdr seq)))))




(define (accumulate proc base seq)
  ;; uforer prosedyren gitt av proc pa hvert element
  ;; i sekvensen proc. Avslutter operasjonen med base.
  (if (null? seq)
      base
      (proc (car seq)
	    (accumulate proc base (cdr seq)))))




(define (sum seq)
  ;; Summerer sammen listeelementene
  (accumulate + 0 seq))





(define (seq-ref n seq)
  ;; Returnerer verdien som ligger pa plass n.
  (define (seq-iter count lst)
    (cond ((null? lst) #f)
	  ((= n count) (car lst))
	  (else (seq-iter (+ 1 count) (cdr lst)))))
  (seq-iter 0 seq))





(define (last-elem seq)
  ;; Finner det siste elementet i listen seq.
  (if (null? (cdr seq))
      (car seq)
      (last-elem (cdr seq))))




(define (seq-len seq)
  ;; returnerer antall elementer i listen.
  (define (seq-iter count lst)
    (if (null? lst)
	count
	(seq-iter (+ 1 count) (cdr lst))))
  (seq-iter 0 seq))




(define (slice from to seq)
  ;; returnerer en ny liste fra 'to' til 'from'
  ;; av den opprinneige.
  (define (make-new a b lst)
    (cond ((= a b) '())
	  ((null? lst) "List-index out of bounds")
	  (else (cons (seq-ref from lst)
		      (make-new (+ 1 a) b (cdr lst))))))
  (make-new from to seq))




(define (num->seq num)
  ;; konverterer nummer til liste.
  (define (weight-iter n seq)
    (if (< n 10)
	(cons n seq)
	(weight-iter (floor (/ n 10))
		     (cons (modulo n 10) seq))))
  (weight-iter num '()))




(define (flatten tree)
  ;; Feller et tre ned til en flat liste.
  (cond ((null? tree) '())
	((pair? (car tree))
	 (append (flatten (car tree))
		 (flatten (cdr tree))))
	(else (cons (car tree)
		    (flatten (cdr tree))))))




(define (all-alike? what where)
  ;; kan brukes til a sjekke om alle elementer i listen
  ;; er identiske.
  (or (null? where)
      (and (what (car where))
	   (all-alike? what (cdr where)))))

;;===================================
;;; Matematiske funksjoner:
;;===================================

;; square:

(define (square n)
  (* n n))


;; Fibonacci:

(define (fib-rek n)
  ;; Rekursiv variant av fibonacci-sekvensen.
  (if (< n 2)
      n
      (+ (fib-rek (- n 1))
	 (fib-rek (- n 2)))))

(define (fib-iter n)
  ;; Iterativ variant av fibonacci-sekvensen.
  ;; c representerer teller, som teller ned
  ;; fra prosisjon i sekvensen av Fibonaccitall.
  (define (iter a b c)
    (if (= 0 c)
	b
	(iter (+ a b) a (- c 1))))
  (iter 1 0 n))

;; Fakultet.

(define (factorial-rek n)
  ;; Rekursiv variant av 
  (if (= 1 n)
      1
      (* n (factorial-rek (- n 1)))))

;; Primtall

(define (is-prime? n)
  (let ((upper-lim (floor (sqrt n))))
    (define (iter count)
      (cond ((> count upper-lim) #t)
	    ((= (modulo n count) 0) #f)
	    (else (iter (+ count 1)))))
    (if (< n 2) #f (iter 2))))



;;===================================
;;; Andre prosedyrer og predikater:
;;===================================

(define (palindrome? seq)
  ;; Sjekker om tallene i listen speiler et palindrom.
  (cond ((null? seq) #t)
	((<= (seq-len seq) 1) #t)
	((equal? (car seq)
		 (last-elem seq))
	 (palindrome? (slice 1 (- (seq-len seq) 1) seq)))
	(else #f)))



(define (biggest! seq)
  ;; Finner det storste elementet i en liste og
  ;; returnerer dette. * Brukt for jeg var klarover
  ;; prosedyren max.
  (let ((big 0))
    (define (iter lst)
      (cond ((null? lst) big)
	    ((< big (car lst))
	     (begin
	       (set! big (car lst))
	       (iter (cdr lst))))
	    (else (iter (cdr lst)))))
    (iter seq)))



;;===================================
;;; Streams, init og prosedyrer:
;;===================================

;;; Init:
;;===============
(define the-empty-stream '())

(define (stream-car stream)
  (car stream))

(define (stream-cdr stream)
  (force (cdr stream)))

(define (stream-null? stream)
  (null? stream))

(define-syntax
  cons-stream
  (syntax-rules ()
    ((cons-stream head tail) (cons head (delay tail)))))


;;; Basic-stream-prosedyrer:
;;===================
(define (show-stream stream . n)
  (define (stream-rec stream i)
    (cond ((= i 0) (display "...\n"))
	  ((stream-null? stream)
	   (newline))
	  (else (display (stream-car stream))
		(display " ")
		(stream-rec (stream-cdr stream) (- i 1)))))
  (stream-rec stream (if (null? n) 15 (car n))))


(define (map-streams proc . argstreams)
  (define (contains-null? lst)
    ;; Sjekker rekursivt liste-element for
    ;; liste-element for den tomme listen
    ;; i argstreams.
    (cond ((null? lst) #f)
	  ((null? (car lst)) #t)
	  (else (contains-null? (cdr lst)))))
  (if (contains-null? argstreams)
      the-empty-stream
      (cons-stream
       (apply proc (map stream-car argstreams))
       (apply map-streams
	      (cons proc
		    (map stream-cdr argstreams))))))


(define (stream-filter pred stream)
  ;; Streamvarianten av filter.
  (cond ((stream-null? stream)
	 the-empty-stream)
	((pred (stream-car stream))
	 (cons-stream (stream-car stream)
		      (stream-filter pred
				     (stream-cdr stream))))
	(else (stream-filter pred (stream-cdr stream)))))


(define (stream-ref stream n)
  ;; Returnerer elementet pa plass n i strommen.
  (if (= n 0)
      (stream-car stream)
      (stream-ref (stream-cdr stream) (- n 1))))


(define (sieve stream)
  ;; sieve = sil.
  ;; Siler ut uonsked elementer fra strommen.
  (cons-stream
   (stream-car stream)
   (sieve (stream-filter
	   (lambda (x)
	     (not (zero? (modulo x (stream-car stream)))))
	   (stream-cdr stream)))))




(define (interleave s1 s2)
  ;; Alternerer mellom to strommer, slik at man er garantert
  ;; at elementer fra begge strommer kommer med i den nye
  ;; strommen uvilkarlig av om den/de er uendelige eller ikke.
  ;; s.341 SICP
  (if (stream-null? s1)
      s2
      (cons-stream (stream-car s1)
		   (interleave s2 (stream-cdr s1)))))

(define (pairs s1 s2)
  (cons-stream
   (list (stream-car s1) (stream-car s2))
   (interleave
    (map-streams (lambda (x) (list (stream-car s1) x))
		 (stream-cdr s2))
    (pairs (stream-cdr s1) (stream-cdr s2)))))


(define (mul-streams s1 s2)
  ;; Multipliserer to streams.
  (map-streams * s1 s2))


(define (add-streams s1 s2)
  ;; Adderer to strommer.
  (map-streams + s1 s2))


(define (triplet-set s1 s2 s3)
  ;; Lager lister med tre elementer 
  (cons-stream
   (list (stream-car s1) (stream-car s2) (stream-car s3))
   (interleave
    (map-streams (lambda (x) (append (list (stream-car s1)) x))
		 (stream-cdr (pairs s2 s3)))
    (triplet-set (stream-cdr s1) (stream-cdr s2) (stream-cdr s3)))))



;;; Grunnleggende mengde-prosedyrer.
;;==================================
(define ones
  ;; Returnerer en uendelig strom av 1.
  (cons-stream 1 ones))

(define (integers-starting-from n)
  ;; Returnerer en uendelig strom av de naturlige tallene
  ;; fra og med n.
  (cons-stream n (integers-starting-from (+ n 1))))

(define nats
  ;; Returnerer mengden av alle naturlige tall.
  (integers-starting-from 1))

(define even-stream
  ;; Genererer en strom av partall.
  (stream-filter even? nats))

(define odd-stream
  ;; Genererer en strom av oddetall.
  (stream-filter odd? nats))

(define primes
  ;; Siler ut alle primtall fra listen av heltall fra 2. 
  (stream-filter is-prime? nats))

(define stream-factorial
  ;; Genererer en strom av alle fakulteter.
  (cons-stream 1 (mul-streams nats stream-fac)))

(define stream-fib
  ;; Generer en strom av alle Fibonacci-tall.
  (cons-stream 0 (cons-stream 1
			    (map-streams
			     +
			     (stream-cdr stream-fib)
			     stream-fib))))




;;===================================
;;; FILE I/O:
;;===================================
(define (file->number f-name)
  (string->number
   (list->string
    (call-with-input-file f-name
      (lambda (input-port)
	(let loop ((x (read-char input-port)))
	  (if (eof-object? x)
	      '()
	      (cons x 
		    (loop (read-char input-port))))))))))




