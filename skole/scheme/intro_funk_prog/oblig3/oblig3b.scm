;;#############################################################################
;;
;;                          Oblig 3a Inf2810
;;                        Christopher N .Storheim
;;
;;                  -- Beklager manglende norske vokaler --
;;
;;#############################################################################
(require r5rs)
(load "evaluator.scm")


;;;Oppgave 1
;; A)
;;

;; (mc-eval

;;  '(define (foo cond else)
;;     (cond ((= cond 2) 0)
;; 	  (else (else cond))))

;;  the-global-environment)

;; => ok




;; Slik fremstaar prosedyren i det globale miljoet etter evaluering

;; #0={{foo ... }
;; {procedure {cond else}
;; 	   {{cond {{= cond 2} 0}
;; 		  {else {else cond}}}} #0#} {.. primitive ..}}


;; Videre evaluering av uttrykkene gitt av oppgaven fremstaar
;; miljoet slik:


;; #0={{{square else cond foo }
;;   {procedure {x} {{* x x}} #0#} {procedure {x} {{/ x 2}} #0#} 3
;;   {procedure {cond else} {{cond {{= cond 2} 0} {else {else cond}}}} #0#} ...



;; Legger merke til at variablene deklarert vha define na ligger i det
;; Globale miljoet, knyttet til tilhoerende prosedyrer eller verdier. 
;; Altsa ihht reglen om evaluering som sier at en variabel er bundet
;; til miljoet den ble evaluert i.

;; Basert pa overnevnte og evalueringsreglene for evaluatoren
;; er det lett a se at folgende blir substituert og evaluert:

;; forste prosedyrekall:

;; (mc-apply
;;    (foo 2 square)
;;  the-global-environment)

;; Substituert:
;; (cond ((= 2 2) 0)
;;    (else (square 2)))

;; Returnerer 0 slik substitusjonen tilsier. 


;; andre prosedyrekall:
;; (mc-apply
;;    (foo 4 square)
;;  the-global-environment)

;; (cond ((= 2 4) 0)
;;    (else (square 4)))
;; Returnerer 16 slik substitusjonen tilsier.


;;
;; (mc-eval
;; '(cond ((= cond 2) 0)
;;    (else (else 4)))
;; the-global-environment)

;; (cond ((= 3 2) 0)
;;   (else ((/ 4 2))))
;; Returnerer 2 slik substitusjonen tilsier.

;; Evaluatoren, nar den evaluerer uttryk av typen define (special forms),
;; bryr seg ikke med hva som ligger etter define. Den vidrebringer uttryket
;; til eval-special-form, som igjen delegerer arbeidet med a legge utrykket
;; og navnet til riktig miljo. Kall pa prosedyren iverksetter
;; look-up av bindinger og substituering av navn med verdier.
;; Disse er styrt av andre regler. Derfor ingen navn-konflikter, og derfor
;; virker substitusjonsreglen som vist over.



;;;Oppgave 2 ==========================>
;; A)

;; Redefinerer primitive-prosedyrer etter a ha lastet inn opprinnelige.
;; @override
(define primitive-procedures
  (list (list 'car car)
	(list 'cdr cdr)
	(list 'cons cons)
	(list 'null? null?)
	(list 'not not)
	(list '+ +)
	(list '- -)
	(list '* *)
	(list '/ /)
	(list '= =)
	(list 'eq? eq?)
	(list 'equal? equal?)
	(list 'display
	      (lambda (x) (display x) 'ok))
	(list 'newline
	      (lambda () (newline) 'ok))
	;;      her kan vi legge til flere primitiver.
	(list '1+
	      (lambda (x) (+ 1 x))) ;; Lagt til
	(list '1-
	      (lambda (x) (- 1 x))) ;; Lagt til
	))

;; Setter opp det globale miljoet for de nye prosedyrene osv.:
(set! the-global-environment (setup-environment))


;; Her blir kroppen til lambda-uttrykket: (+ 1 x)/(- 1 x) definert av treff
;; pa 1+/- forsoekt 'applied' pa parameterene definert av cdr av
;; prosedyrelisten sendt til mc-eval ved init-kall til mc-eval.
(mc-eval '(1+ 2) the-global-environment)
;; => 3
(mc-eval '(1- 2) the-global-environment)
;; => 1



;;-----------------------------------------------------------------------------



;;;Oppgave 2
;; B)

(define (install-primitive! name-tag proc)
  (let ((new-primitive (list (cons name-tag (list proc)))))
    (define (append-to-primitives lst)
      ;; Loeper gjennom listen av primitive prosedyrer og legger til denne
      ;; prosedyren til slutten, samtidig blir the-global-environment oppdatert
      ;; til a inneholde denne prosedyren med et kall til define-variabel!
      ;; med name-tag, prosedyren og t-g-e som formalparametere.
      (if (null? (cdr lst))
	  (begin
	    (set-cdr! lst new-primitive)
	    (define-variable!
	      name-tag
	      (cons 'primitive (list proc))
	      the-global-environment))
	  (append-to-primitives (cdr lst)))
      name-tag)
    (append-to-primitives primitive-procedures)))

;; Opprettelse og test:
(install-primitive! 'square (lambda (x) (* x x)))

(mc-eval '(square 4) the-global-environment)



;;-----------------------------------------------------------------------------
;;; Oppgave 3
;;  A)

;; @override
(define (eval-special-form exp env)
  (cond ((quoted? exp) (text-of-quotation exp))
        ((assignment? exp) (eval-assignment exp env))
        ((definition? exp) (eval-definition exp env))
        ((if? exp) (eval-if exp env))
	((and? exp) (eval-and exp env)) ;;<- Lagt til
	((or? exp) (eval-or exp env));;<- Lagt til
	((lambda? exp)
         (make-procedure (lambda-parameters exp)
                         (lambda-body exp)
                         env))
        ((begin? exp) 
         (eval-sequence (begin-actions exp) env))
        ((cond? exp) (mc-eval (cond->if exp) env))))

;; @override
(define (special-form? exp)
  (cond ((quoted? exp) #t)
        ((assignment? exp) #t)
        ((definition? exp) #t)
        ((if? exp) #t) 
        ((lambda? exp) #t)
        ((begin? exp) #t)
        ((cond? exp) #t)
	((and? exp) #t)  ;; <= Lagt til
	((or? exp) #t)   ;; <= Lagt til
        (else #f)))



;; AND flagg-sjekk-proc og eval======>:

(define (and? exp)
  ;; 'Standard' type-sjekk
  (tagged-list? exp 'and))

(define (eval-and exp env)
  ;; Evaluerer rekursivt alle predikater i listen av uttryk.
  ;; Returnerer false ved foerste intruffene ikke-oppfylt predikat.
  ;; For alle andre tilfeller true.
  (define (iter pred-lst)
    (if (null? pred-lst)
	#t
	(if (false? (mc-eval (car pred-lst) env))
	    #f
	    (iter (cdr pred-lst)))))
  (iter (cdr exp)))




;;; OR flagg-sjekk-proc og eval ======>:

(define (or? exp)
  (tagged-list? exp 'or))

(define (eval-or exp env)
  ;; Evaluerer rekursivt alle predikater i listen av uttryk.
  ;; Returnerer true  ved foerste intruffene oppfylt predikat.
  ;; For alle andre tilfeller false.
  (define (iter pred-lst)
    (if (null? pred-lst)
	#f
	(if (true? (mc-eval (car pred-lst) env))
	    #t
	    (iter (cdr pred-lst)))))
  (iter (cdr exp)))




;;; Test AND ======>
(mc-eval '(and (= 3 3) (= 2 2) (and #t #t)) the-global-environment) ;;=> #t
(mc-eval '(and (= 3 3) (= 2 2) (and #f #t)) the-global-environment) ;;=> #f
(mc-eval '(and #f #t) the-global-environment) ;;=> #f
(mc-eval '(and) the-global-environment) ;;=> #t

;;; Test OR ======>
(mc-eval '(or (= 3 3) (= 2 2) (or #t #t)) the-global-environment) ;;=> #t
(mc-eval '(or (= 3 1) (= 1 2) (or #t #f)) the-global-environment) ;;=> #t
(mc-eval '(or #f #f) the-global-environment) ;;=> #f
(mc-eval '(or) the-global-environment) ;;=> #f



;;-----------------------------------------------------------------------------
;;; Oppgave 3
;;  b)


;; @override
(define (eval-special-form exp env)
  (cond ((quoted? exp) (text-of-quotation exp))
        ((assignment? exp) (eval-assignment exp env))
        ((definition? exp) (eval-definition exp env))
        ((if? exp) (eval-if-else exp env));;<- Lagt til
	((and? exp) (eval-and exp env)) ;;<- Lagt til
	((or? exp) (eval-or exp env));;<- Lagt til
	((lambda? exp)
         (make-procedure (lambda-parameters exp)
                         (lambda-body exp)
                         env))
        ((begin? exp) 
         (eval-sequence (begin-actions exp) env))
        ((cond? exp) (mc-eval (cond->if exp) env))))

;; @override
(define (special-form? exp)
  (cond ((quoted? exp) #t)
        ((assignment? exp) #t)
        ((definition? exp) #t)
        ((if? exp) #t) 
        ((lambda? exp) #t)
        ((begin? exp) #t)
        ((cond? exp) #t)
	((and? exp) #t)  ;; <= Lagt til
	((or? exp) #t)   ;; <= Lagt til
        (else #f)))

;; If-Then-Elsif-Else ======>:

(define (found-if? exp)
  (or (eq? (car exp) 'if)
      (eq? (car exp) 'elsif)))

(define (found-else? exp)
  (eq? (car exp) 'else))

(define (else-exp exp)
  (cadr exp))

(define (ifs-consequent exp)
  (cadddr exp))

(define (if-or-elsif-pred exp)
  (cadr exp))


(define (eval-if-else exp env)
  ;; Kalt den eval-if-else for ikke a overkjore if for resten
  ;; av oppgaven.
  ;; Losning. Base: treff pa else -> eval uttrykket
  ;; Treff pa if eller elsif OG utrykket evaluerer til true.
  ;; Evaluer konsekvensen. Loop til treff pa else eller tom liste.
  (cond ((null? exp) 'false)
	((found-else? exp)
	 (if (null? (cddr exp))
	     (mc-eval (else-exp exp) env)
	      "ELSE-clause isn't last!"))
	((and (found-if? exp)
	      (mc-eval (if-or-elsif-pred exp) env))
	 (mc-eval (ifs-consequent exp) env))
	(else (eval-if-else (cdr exp) env))))

;; TESTER:
(mc-eval '(if (= 3 1)
	      then (+ 3 1)
	      elsif (= 2 3)
	      then (+ 9 1)
	      else (+ 8 9)) the-global-environment)
;; => 17

(mc-eval '(if (= 3 1)
	      then (+ 3 1)
	      elsif (= 2 3)
	      then (+ 9 1)) the-global-environment)
;; => false
;; ingen else

(mc-eval '(if (= 3 1)
	      then (+ 3 1)
	      else (+ 2 1)
	      elsif (= 2 3)
	      then (+ 9 1)) the-global-environment)
;; => "ELSE-clause isn't last!" 
;; Provde med (error ""), men den bare henger...






;; Denne kan du se bort ifra! Jeg "tror" jeg misforstod oppgaven initielt,
;; men lar den staa som en kuriositet over hva det kan lede til nar
;; man fjerner meta fra sirkulaer og bare ender i sirkel...
;; Fungerer ikke etter at if-er redefinert over (sirkel..igjen)..

(define (If? exp)
(tagged-list? exp 'If))

(define (if-else->cond exp)
  (expand-clauses (filter-conds exp)))

(define (filter-conds lst)
  ;; Versjon #1349
  ;; Denne gang: gjor uttrykket om til cond-format som evaluatoren kan
  ;; tygge pa. Merk 'I' i If sa den ikke biter seg selv i halen..
  (cond ((null? lst) lst)
	((eq? (car lst) 'else)
	 (cons (cons (car lst) (cdr lst)) (filter-conds (cdr lst))))
	((or (eq? (car lst) 'If)
	     (eq? (car lst) 'elsif))
	 (cons (cons (cadr lst) (list (cadddr lst)))
	       (filter-conds (cdr lst))))
	(else (filter-conds (cdr lst)))))

(expand-clauses (filter-conds
		   '(If (= 1 3)
			then (+ 3 1)
			elsif (= 3 3)
			then (+ 9 1)
			else (+ 8 9))))


;;-----------------------------------------------------------------------------

;;;Oppgave 3
;; C)

;; @override
(define (eval-special-form exp env)
  (cond ((quoted? exp) (text-of-quotation exp))
        ((assignment? exp) (eval-assignment exp env))
        ((definition? exp) (eval-definition exp env))
        ((if? exp) (eval-if exp env))
	((and? exp) (eval-and exp env))
	((or? exp) (eval-or exp env))
	((let? exp)
	 (mc-eval (let->lambda exp) env)) ;;<-lagt til
	((lambda? exp)
         (make-procedure (lambda-parameters exp)
                         (lambda-body exp)
                         env))
        ((begin? exp) 
         (eval-sequence (begin-actions exp) env))
        ((cond? exp) (mc-eval (cond->if exp) env))))

;; @override
(define (special-form? exp)
  (cond ((quoted? exp) #t)
        ((assignment? exp) #t)
        ((definition? exp) #t)
        ((if? exp) #t) 
        ((lambda? exp) #t)
        ((begin? exp) #t)
        ((cond? exp) #t)
	((and? exp) #t)
	((or? exp) #t) 
	((let? exp) #t);;<- Lagt til
        (else #f)))



;; Transformasjon:
;; (let ((x 3) (y 4)) (<body>)) == ((lambda (x y) (<body>)) 3 4)
;;            ^
;;           expr

(define (let? exp)
  ;; Er uttrykket av type let?
  (tagged-list? exp 'let))



(define (let-expression exp)
  ;; Returnerer listen av uttrykk.
  (cadr exp))



(define (let-body exp)
  ;; Returnerer uttrykket i kroppen.
  (cddr exp))


(define (let->lambda exp)
  ;; (('lambda (parameter) (kropp)) (parameterverdi))
  ;; Lager et lambda-uttykk av let-uttrykket og evaluerer umiddelbart
  ;; uttrykket i miljoet det ble opprettet i. Slik vil bindingen
  ;; mellom navn og verdi eksistere i miljoet,
  (cons (make-lambda (collect-let-vars (let-expression exp))
		     (let-body exp))
	(collect-let-expr (let-expression exp))))


(define (variable-name expr)
  ;; Abstraksjon for lesbarhet
  (caar expr))


(define (variable-expr expr)
  ;; Abstraksjon for lesbarhet
  (cadar expr))


(define (collect-let-vars expr-list)
  ;; Loper gjennom listen over var-navn/uttrykk par og
  ;; henter ut var-navn.
  (if (null? expr-list)
      '()
      (cons (variable-name expr-list)
	    (collect-let-vars (cdr expr-list)))))


(define (collect-let-expr expr-list)
  ;; Loper gjennom listen over var-navn/uttrykk par og
  ;; henter ut uttrykket.
  (if (null? expr-list)
      '()
      (cons (variable-expr expr-list)
	    (collect-let-expr (cdr expr-list)))))


;; Funksjonstest:
(mc-eval '(define x (let ((a 6) (b 4)) (+ a b)))
	 the-global-environment)

(mc-eval 'x the-global-environment)




;;-----------------------------------------------------------------------------

;;;Oppgave 3
;; D)

;; Jeg valgt a kalle denne math-let da jeg jobbet litt frem og tilbake
;; mellom de forskjellige oppgavene.

;; @override
(define (eval-special-form exp env)
  (cond ((quoted? exp) (text-of-quotation exp))
        ((assignment? exp) (eval-assignment exp env))
        ((definition? exp) (eval-definition exp env))
        ((if? exp) (eval-if exp env))
	((and? exp) (eval-and exp env))
	((or? exp) (eval-or exp env))
	((let? exp) (eval-let exp))
	((math-let? exp) ;;<-lagt til
	 (mc-eval (math-let->lambda exp)
		  env)) 
	((lambda? exp)
         (make-procedure (lambda-parameters exp)
                         (lambda-body exp)
                         env))
        ((begin? exp) 
         (eval-sequence (begin-actions exp) env))
        ((cond? exp) (mc-eval (cond->if exp) env))))


;; @override
(define (special-form? exp)
  (cond ((quoted? exp) #t)
        ((assignment? exp) #t)
        ((definition? exp) #t)
        ((if? exp) #t) 
        ((lambda? exp) #t)
        ((begin? exp) #t)
        ((cond? exp) #t)
	((and? exp) #t) 
	((or? exp) #t)  
	((let? exp) #t)
	((math-let? exp) #t)  ;; <= Lagt til
        (else #f)))

;;; Opprinneligform:
;; (let <var1> = <exp1> and <var2> = <exp2> in <body>)

;;; Ma-ha-form:
;; ((lambda (<var1> <var2>) (<body>)) <exp1> <exp2>)


(define (math-let? exp)
  ;; Er uttrykket av type math-let?
  (tagged-list? exp 'math-let))



(define (math-let->lambda exp)
  ;; (('lambda (parameter) (kropp)) (parameterverdi))
  ;; Lager et lambda-uttykk av let-uttrykket og evaluerer umiddelbart
  ;; uttrykket i miljoet det ble opprettet i. Slik vil bindingen
  ;; mellom navn og verdi eksistere i miljoet nar det skal brukes.
  (cons (make-lambda (find-var-names exp)
		     (find-body exp))
	(find-var-val exp)))



(define (find-body exp)
  ;; Henter ut uttrykket etter key-word 'in'
  (if (member 'in exp)
      (list (cadr (member 'in exp)))
      (error "No body in let-expression")))



(define (find-var-val exp)
  ;; henter ut variabel verdiene i rekkefolge.
  ;; Baserer seg pa at disse folger etter '='-tegn.
  (cond ((null? exp) '())
	((equal? (car exp) '=)
	 (cons (cadr exp) (find-var-val (cdr exp))))
	(else (find-var-val (cdr exp)))))



(define (find-var-names exp)
  ;; Samme som over, men henter ut det forran '='-tegnet.
  (cond ((equal? (car exp) 'in) '())
	((eq? (cadr exp) '=)
	 (cons (car exp) (find-var-names (cdr exp))))
	(else (find-var-names (cdr exp)))))



;;; Funksjonstest:
(mc-eval '(define y (math-let x = 3 and y = 4 in (+ x y)))
	 the-global-environment)
;;=> ok

(mc-eval 'y the-global-environment)
;;=> 7



;;-----------------------------------------------------------------------------

;;;Oppgave 3
;; E)

;; Her melder jeg pass. Har en ide, men den leder meg ingensteds hen.
;; Legger ved pabegynt mulig loesning.


;;(while <pred> <body>)


(install-primitive! 'while (lambda (pred body)
			     ((lambda (self) (self self))
			      (lambda (self)
				(if (> x 0) ;;<- pred
				    (begin 
				      (display body)  ;;<- (body_
				      (newline)       ;;<-  _body_
				      (set! x (- x 1));;<- _body)
  				     (self self))
				    'done)))))

;; Ideen var noe slikt:
(define x 10)
(mc-eval '(while "pred not fullfilled" "Do something") the-global-environment)
;; Men noe skjer over mon forstand slik at hvis jeg putter inn pred for
;; if-prediaktet sa faar jeg tilbakemld om at f.eks unbound variable '>' 
