(define (clean-id name) (regexp-replace "[!?<>]" name "_"))

(define (write-header locale title htmlfile cssfilename)
  (define (line-loop in out)
    (let ((ret (read-line in)))
      (cond 
        ((not (eof-object? ret))
         (fprintf out "~a~n" ret)
         (line-loop in out)))))

  (fprintf htmlfile "<?xml version=\"1.0\" encoding=\"UTF-8\"?>~n<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">~n<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"~a\" lang=\"~a\">~n<head>" locale locale)
  (fprintf htmlfile "<title>~a</title>~n" title)
  (fprintf htmlfile "<style type=\"text/css\"><!--~n")
  (let ((cssfile (open-input-file cssfilename)))
    (line-loop cssfile htmlfile)
    (close-input-port cssfile))
  (fprintf htmlfile "--></style>~n</head>~n<body>~n"))
  
(define (write-footer htmlfile)
  (fprintf htmlfile "</body>~n</html>~n"))

(define (write-function name args returns desc example htmlfile)
  (if (string=? args "")
      (fprintf htmlfile "<h2><a name=\"~a\">(~a)</a></h2>" (clean-id name) name)
      (fprintf htmlfile "<h2><a name=\"~a\">(~a ~a)</a></h2>~n" (clean-id name) name args))
  (fprintf htmlfile "<p><b>Returns</b> ~a</p>~n" returns)
  (fprintf htmlfile "<p>~a</p>~n" desc)
  (fprintf htmlfile "<p><b>Example</b></p>~n")
  (fprintf htmlfile "<pre>~a</pre>~n" example))

(define (write-section name desc example htmlfile)
  (fprintf htmlfile "<h1>~a</h1>~n" name)
  (fprintf htmlfile "<h3>Description</h3>~n")
  (fprintf htmlfile "<p>~a</p>~n" desc)
  (cond 
    ((not (string=? example ""))
     (fprintf htmlfile "<h3>Example</h3>~n")
     (fprintf htmlfile "<pre>~a</pre>~n" example))))

(define (write-section-index name htmlfile)
  (fprintf htmlfile "<h2><a href=\"~a.html\">~a</a></h2>" name name))

(define (write-locale locale htmlfile)
  (fprintf htmlfile "<a href=\"~a/index.html\">~a</a> " locale locale))

(define (write-all-function name section htmlfile)
  (fprintf htmlfile "<a href=\"~a.html#~a\">~a</a> " section (clean-id name) name))

(define (write-functionlist-start htmlfile)
  (fprintf htmlfile "<p>"))

(define (write-functionlist-end htmlfile)
  (fprintf htmlfile "</p>"))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define (parse-function functionlist htmlfile allhtmlfile section)
  (cond
    ((not (null? functionlist))
     (let ((function (cadr (car functionlist))))
       (write-all-function (car (car functionlist)) section allhtmlfile)
       (write-function (car (car functionlist)) 
                       (car function) 
                       (cadr function) 
                       (caddr function) 
                       (cadddr function) 
                       htmlfile))
     (parse-function (cdr functionlist) htmlfile allhtmlfile section))))

(define (parse-section sectionlist indexhtmlfile locale)
  (cond
    ((not (null? sectionlist))
     (let ((section (cadr (car sectionlist)))
           (sectionname (car (car sectionlist))))
       (write-section-index sectionname indexhtmlfile)
       (let ((htmlfile (open-output-file (string-append locale "/" sectionname ".html") 'replace)))
         (write-header locale sectionname htmlfile "fluxusdoc.css")
         (write-section sectionname (car section) (cadr section) htmlfile)
         (write-functionlist-start indexhtmlfile)
         (parse-function (caddr section) htmlfile indexhtmlfile sectionname)
         (write-functionlist-end indexhtmlfile)
         (write-footer htmlfile)
         (close-output-port htmlfile)))
     (parse-section (cdr sectionlist) indexhtmlfile locale))))
 
(define (parse-locale helpmap htmlfile)
  (cond
    ((not (null? helpmap))
     (let ((locale (car (car helpmap))))
       (if (not (directory-exists? locale))
           (make-directory locale))
       (write-locale locale htmlfile)
       (let ((htmlfile (open-output-file (string-append locale "/index.html") 'replace)))
         (write-header locale "Section Index" htmlfile "fluxusdoc.css")
         (parse-section (cadr (car helpmap)) htmlfile locale)
         (write-footer htmlfile)
         (close-output-port htmlfile))
       (parse-locale (cdr helpmap) htmlfile)))))


(let ((file (open-input-file "helpmap.scm")))
  (let ((htmlfile (open-output-file "index.html" 'replace)))
    (write-header "en" "Fluxus Documentation 0.13" htmlfile "fluxusdoc.css")
    (fprintf htmlfile "<h1>Fluxus Documentation 0.13</h1>~n")
    (fprintf htmlfile "<p>Extracted and html-ised from the runtime documentation system</p>~n")
    (fprintf htmlfile "<p>Available languages: ")
    (parse-locale (read file) htmlfile)
    (close-input-port file)
    (fprintf htmlfile "</p>")
    (write-footer htmlfile)
    (close-output-port htmlfile)))