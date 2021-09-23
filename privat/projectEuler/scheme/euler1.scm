(require r5rs)
(load "projectTools.scm")




;;============================= Oppgave 1 =============================
;; Summere alle tall fra 0 til 1000 som er faktorer av enten 3
;; eller 5.

;; Filtrer ut alle tall som oppfyller predikatet, summerer sa disse.
(define (fact-of-3-or-5 x)
  (cond ((= (modulo x 3) 0) #t)
	((= (modulo x 5) 0) #t)
	(else #f)))

(sum (filter fact-of-3-or-5 (range 1 1000 1)))

;; => 233168
;;===========
;; (time (sum (filter fact-of-3-or-5 (range 1 1000 1))))
;; cpu time: 0 real time: 1 gc time: 0





;;============================= Oppgave 2 =============================
;; Finn summen av alle par-Fibonacci-tall hvor summen
;; totalt ikke overgaar 4 000 000.

;; Iterer over alle fibonaccitallene og tester om de er partall. Hvis sa
;; legges de til summen. Nar summen ovre begrensning, returneres tallet.

(define (sum-even-fib stop)
  (define (iter sum n)
    (let ((fib-of-n (fib-iter n)))
      (cond ((> sum stop) sum)
	    ((even? fib-of-n)
	     (iter (+ sum fib-of-n) (+ 1 n)))
	    (else (iter sum (+ n 1))))))
  (iter 0 1))

(sum-even-fib 4000000)

;;=> 4613732
;;==========
;; (time (sum-even-fib 4000000))
;; Rekursiv fibonacci: cpu time: 464 real time: 461 gc time: 0
;; Iterativ fibonacci: cpu time: 0 real time: 0 gc time: 0





;;============================= Oppgave 3 =============================
;; Finn den storste primtall-faktoren til tallet 600851475143

;; Antagelse1: Det finnes ingen faktorer av tallet som er storre enn
;; halvparten av tallet selv, da produktet ville ha oversteget tallet.
;; Antagelse2: Antar at tallet kan beskrives som produktet av to faktorer
;; n og m. Hvis sa kan et av faktorene aldri overstige verdien av sin egen
;; rot. Dette fordi n^1/2 * m^1/2 = tallet selv.
;; Pa bakgrunn av de overnevnte antar jeg at faktoren maa ligge mellom 2
;; (det forste primtallet) og roten av tallet selv.

;; Sa bygger en liste med antatte faktorer og filtrer denne for tall hvor
;; divisjonen med tallet gaar opp. Sa filterer jeg ut alle faktorer av
;; disse som er primtall, for jeg er ikke garantert at probable faktor er
;; et primtall selv. det storste ligger sa foerst i listen av tall.

(define (largest-prime-factor n)
  (let ((probable-factor (ceiling (sqrt n))))
    (car (filter is-prime? (filter
		  (lambda (x) (= (modulo n x) 0))
		  (range probable-factor 2 -1))))))

(largest-prime-factor 600851475143)

;;=> 6857.0
;==========
;; (time (largest-prime-factor 600851475143))
;;cpu time: 690 real time: 687 gc time: 478





;;============================= Oppgave 4 =============================
;; Finn det stoerste produktet, av a multiplisere 2
;; tre-siffrede tall, som er et palindrom.


;; Jeg begynner med de storste mulig sifrene 999, men det spiller ingen rolle
;; for hvordan losningen genereres. Det vil si det klart finnes en bedre mate.
;; Indre lokken dekrementerer top1 for hver iterasjon, samtidig bygges en liste
;; med alle hittils funnede palindromer. Nar top en nar det laveste 3 siffrede
;; nummeret kalles ytre-prosedyren med dekrementert top2 og top1 holdes statisk
;; helt frem til top1 nar basistilfellet. Da returneres listen og sendes vidre
;; for utdrag av det storste nummert funnet.

(define (find-largest-palindrome-product top1 top2)
  (define (iter low high)
    (cond ((= top2 100) '())
	  ((= low 100)
	   (find-largest-palindrome-product top1 (- top2 1)))
	  ((palindrome? (num->seq (* low high)))
	   (cons (* low high) 
		 (iter (- low 1) high)))
	  (else (iter (- low 1) high))))
  (iter top1 top2))

(apply max (find-largest-palindrome-product 999 999))

;; => 906609
;;===========
;;(time (biggest! (find-largest-palindrome-product 999 999)))
;; cpu time: 773 real time: 770 gc time: 3





;;============================= Oppgave 5 =============================
;; 2520 er det minste tallet som er delelig pa samtlige naturlige tall
;; fra og med 1 til og med 10.
;; Finn det minste tallet som er delelig pa alle tall opp til og med 20

;; Her er en ganske rett frem losning, men jeg har tatt en snarvei
;; som jeg sliter med  bevise at er riktig. Antagelsen min gar ut pa at,
;; siden vi vet at det minste tallet som gar opp i alle tall fra 1-10
;; er 2520, er magefolelsen min at tallet jeg leter etter er en faktor
;; av dette tallet. Derfor forkaster jeg alle tall som ikke er en faktor
;; av 2520 og regner kun ut pa de resterende. Dette gir riktig resultat
;; for akkurat denne oppgaven, men om det stemmer for alle tall vet jeg
;; ikke. Med bakgrunn i antagelsen tar jeg ogsa en ny antagelse om at jeg
;; kun trenger a sjekke fra 11 - 20 da de foregaaende allerede er sjekket
;; vha tallet 2520.

(define (divisible-up-to n from)
  (define (iter count)
    (cond ((not (zero? (modulo count 2520)))
	   (iter (+ count 1)))
	  ((zero? (sum (map (lambda (y)
			      (modulo count y))
			    (range 11 (+ 1 n) 1)))) count)
     (else (iter (+ count 1)))))
  (iter from))

(divisible-up-to 20 2520)

;; 232792560
;; =========
;; (time (divisible-up-to 20 2520))
;; cpu time: 3727 real time: 3730 gc time: 0





;;============================= Oppgave 6 =============================
;; Finn differansen mellom summen av kvadratene av de forste 100
;; naturlige tallene og kvadratet av summen av disse.

;; Losning:
;; Her kommer hoyereordensprosedyrer og funkjsonellprogrammering
;; absolutt til sin rette og oppgaven gjor det som star i koden.

(define (diff-sum-square from to)
  (let ((number-range (range from to 1)))
    (- (square (sum number-range))
       (sum (map square number-range)))))

(diff-sum-square 1 101)

;; 25164150
;; ========
;; (time (diff-sum-square 1 101))
;; cpu time: 0 real time: 0 gc time: 0




;;============================= Oppgave 7 =============================
;; Finn det 10001. primtallet.

;; Losning:
;; Her ville jeg bruke streams, sa stream prosedyrer ble lagt til
;; i biblioteket. Bruke stream-ref pa elementnummer > 1000 spiser
;; all minne pa maskinen 8GB - 512mb sys-minne saa noe er rart.
;; Derfor ble det lagt til en rett frem iterativ prosedyre for jobben.

;; Denne iterer gjennom alle tall fra null og inkrementerer
;; prime-cnt variablen hver gang den snubler over et primtall.
;; Nar prime-cnt variablen er lik n returneres primtallet.

(define (prime-number n)
  (stream-ref primes (- n 1)))

(define (prime-number-iter n)
  (define (prime-iter cnt prime-cnt prime-num)
    (cond ((= prime-cnt n)
	   prime-num)
	  ((is-prime? cnt)
	   (prime-iter (+ cnt 1) (+ prime-cnt 1) cnt))
	  (else (prime-iter (+ cnt 1) prime-cnt  prime-num))))
  (prime-iter 0 0 2))


(prime-number-iter 10001)
;; 104743
;; ======
;; (time (prime-number-iter 10001))
;; cpu time: 106 real time: 105 gc time: 4

;; Alternativt med strommer:
(prime-number 10000)
(time (prime-number 10000))
;; cpu time: 0 real time: 1 gc time: 0



;;============================= Oppgave 8 =============================
;; Finn det stoerste produktet av a multiplisere 13 etterfoelgende siffer
;; i et 1000 siffer langt tall gitt i oppgaven.

;; I python loeste jeg denne oppgaven ved a ga 13 plasser inn i listen, for
;; sa a multiplisere 'halen', for siden a oeke med 1 inntil den tomme listen.
;; Det gjorde jeg fordi jeg ikke skulle treffe 'veggen' ved a multiplisere
;; tallene forover.
;; Det var litt mer utfordrene her, sa losningen ble litt smartere. Fant ut
;; at siste elementet er garantert a bli multiplisert med de foregaende 12
;; hvis operasjonen stopper 13 elementer for slutten av listen. Ganske
;; naturlig hvis man tenker litt etter og tenker matematikk fremfor data-
;; kontainere.

(define huge-num (num->seq (file->number "oppgave8.dat")))

(define (largest-product lst step)
  (let ((list-len (length lst)))
    (define (iter seq cnt)
      (if (= cnt (- list-len step))
	  '()
	  (cons (accumulate * 1 (slice 0 step seq))
		(iter (cdr seq) (+ cnt 1)))))
    (apply max (iter lst 0))))


(largest-product huge-num 13)
;; 23514624000
;; ===========
;;(time (largest-product huge-num 13))
;; cpu time: 3 real time: 3 gc time: 0




;;============================= Oppgave 9 =============================
;; Det skal finnes noyaktg 1 Pythagorisk triplet hvor summen av
;; a+b+c = 1000, finn produktet a*b*c.
;; En pyt.triplet er 3 naturlige nummer hvor a<b<c og a*a + b*b = c*c


;; Denne fungerer matematisk-optimalt for oppgaven, men bakdelen er
;; minnebruk og tidsbruk. Det leder til at den ikke far generert
;; flere enn 6-7 tripler for den bukker under.
(define pythagorian-triplet-stream
  ;; Exercise 3.69 SICP
  (let ((nats-triplets (triplet-set nats nats nats)))
    (stream-filter ;;filtrer ut:  a*a + b*b = c*c
     (lambda (x)
       (= (square (caddr x))  
	  (+ (square (cadr x))   
	     (square (car x))))) 
     nats-triplets)))


(define (prod-pythagorian-triplet num stream)
  ;; strommen kommer pa formen ((a b c) ... ) sa nar summen a+b+c = 1000
  ;; returneres productet abc, ellers rekurseres det til neste liste.
  (if (= num (apply + (car stream)))
      (apply * (car stream))
      (prod-pythagorian-triplet num (stream-cdr stream))))



(define (prod-pythagorian-triplet2 num)
  ;; Denne baserte jeg pa wikipedia-siden over formler
  ;; for a finne pythagorian-triplets, Fibonaccis metode.
  ;; Den treffer aldri base case for 1000 og crasher til
  ;; slutt, dette skyldes at den genererer odde 'a' fra
  ;; 1 og ikke har et definert tak for 'b' og 'c'.
  (let ((final? (lambda (x) (= x num)))
	 (odd-numbers (filter odd? (range 1 100000 1))))
    (define (iter-odd cnt)
      (let* ((a (seq-ref cnt odd-numbers))
	     (n (/ (+ (square a) 1) 2))
	     (b (sqrt (sum (slice 0 (- n 1) odd-numbers))))
	     (c (sqrt (sum (slice 0 n odd-numbers)))))
	(if (final? (+ a b c))
	    (* a b c)
	    (iter-odd (+ cnt 1)))))
    (iter-odd 1)))



(define (prod-pythagorian-triplet3)
  ;; Forsok 3: c = 1000 - a - b => a*a + b*b = (1000-a-b)(1000-a-b)
  ;; base-case =>    500000 - 1000a - 1000b + a*b == 0
  (let ((done? (lambda (x y)
		 (zero? (- (+ 500000 (* x y))
			   (* 1000 x) (* 1000 y))))))
    (define (iter a b)
      (cond ((done? a b)
	     (* a b (- 1000 a b)))
	    ((and (= a 500) (= b 500)) #f)
	    ((= a 500) (iter 1 (+ b 1)))
	    (else (iter (+ a 1) b))))
    (iter 1 1)))


(prod-pythagorian-triplet3)
;;=> 31875000
;;===========
;; (time (prod-pythagorian-triplet3))
;; cpu time: 3 real time: 2 gc time: 0



;;============================ Oppgave 10 =============================
;; Finn summen av alle primtall under 2 000 000.

(define (sum-primes-below n)
  (define (prime-iter cnt prime-sum)
    (cond ((= cnt n) prime-sum)
	  ((is-prime? cnt)
	   (prime-iter (+ cnt 1) (+ prime-sum cnt)))
	  (else (prime-iter (+ cnt 1) prime-sum))))
  (prime-iter 2 0))

(sum-primes-below 2000000)
;;= 142913828922
;;==============
;;(time (sum-primes-below 2000000))
;; cpu time: 5510 real time: 5518 gc time: 7


(define (stream-sum-below n stream)
  ;; Gjoer nok jobben, men bruker lenger
  ;; tid og minne enn jeg har tilgjengelig.
  (define (sum-stream cnt s curr-sum)
    (if (= cnt n)
	curr-sum
	(sum-stream (+ cnt 1)
		    (stream-cdr s)
		    (+ curr-sum (stream-car s)))))
  (sum-stream 0 stream 0))



;;============================ Oppgave 11 =============================
(define grid (make-grid 20 20 (load "oppgave11.dat")))

(define (prod-N->S grid) "1")
(define (prod-W->E grid) "2")
(define (prod-NW->SE grid "3"))
(define (prod-SW->NE grid) "4")


;;============================ Oppgave 12 =============================
;; Finn verdien til det forste trianguleare tallet med over 500
;; divisorer

;; loser denne ved a generere en strom av triangulaere tall basert
;; definisjonen av triangulaere tall funnet pa Wikipedia-artikkelen.
;; Disse tallene kjores gjennom en algoritme som stykker opp tallet
;; i sine divisorer og returnerer antallet.



(define triangular-num-stream
  (map-streams (lambda (n) (/ (* n (+ n 1)) 2)) nats))

(define (number-of-divisors num)
  ;; TODO: Forbedre, er ikke helt konsis.
  (define (iter i limit sum)
    (cond ((>= i limit) sum)
	  ((zero? (modulo num i))
	   (if (not (= limit i))
	       (iter (+ i 1) (/ num i) (+ sum 2))
	       (iter (+ i 1) (/ num i) (+ sum 1))))
	  (else (iter (+ i 1) limit sum))))
  (if (is-prime? num) 2 (iter 1 num 0)))


(define (num-divisors-above limit tria-numbers)
  (if (>= (number-of-divisors (stream-car tria-numbers)) limit)
      (stream-car tria-numbers)
      (num-divisors-above limit (stream-cdr tria-numbers))))

(num-divisors-above 500 triangular-num-stream)
;;= 76576500
;;==========
;; (time (num-divisors-above 500 triangular-num-stream))
;; cpu time: 1157 real time: 1157 gc time: 0


;;============================ Oppgave 13 =============================
;; Oppgaven gar ut pa a hente ut de forste 10 sifferene i summen av
;; 100 50-sifffer store tall.

;; Denne er ganske rett frem, nummerene befinner seg i en file pa
;; listeform. Legger sammen  alle nummerene gjoer det om til en liste
;; og derifra henter ut de foerste ti sifferene i summen.

(define hundred-numbers (load "oppg13.dat"))

(slice 0 10 (num->seq (apply + hundred-numbers)))

;; => {5 5 3 7 3 7 6 2 3 0}
;;=========================
;;(time (slice 0 10 (num->seq (apply + hundred-numbers))))
;; cpu time: 0 real time: 0 gc time: 0



;;============================ Oppgave 14 =============================
;; Finn tallet under 1000000 som gir den lengste Collatzkjeden.

;; Collatz sekvenser er rekursive i sin definisjon ved at de,
;; saa langt det er bevist, alltid slutter med tallet 1.
;; Saa a lage en prosedyre som accumulerer collats-tall og teller
;; dem er helt trivielt. Likesaa er rettfrem-loesningen paa hvordan
;; jeg finner det stoerste tallet.


(define (collatz-sequence-count num)
  (define (iter num cnt)
    (cond ((= 1 num) cnt)
	  ((zero? (modulo num 2))
	   (iter (/ num 2) (+ cnt 1)))
	  (else (iter (+ (* num 3) 1) (+ cnt 1)))))
  (iter num 1))

(define (largest-collatz-seq-num limit)
  (define (iter n biggest res)
    (let ((result (collatz-sequence-count n)))
      (cond ((= n limit) res)
	    ((> result biggest)
	     (iter (+ n 1) result n))
	    (else (iter (+ n 1) biggest res)))))
  (iter 1 0 0))

(largest-collatz-seq-num 1000000)
;; => 837799
;; =======
;; (time (largest-collatz-seq-num 1000000))
;; cpu time: 3516 real time: 3512 gc time: 0



;;============================ Oppgave 14 =============================

(define (lettice-path row col num-choises)
  (let* ((min-path (+ row col))
	(comb (/ min-path num-choises)))
    (/ (factorial-rek min-path)
       (* (factorial-rek comb)
	  (factorial-rek (- min-path comb))))))

(lettice-path 20 20 2)
