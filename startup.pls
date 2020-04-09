(begin
    (define make-point (lambda (x y) (set-property "size" 0 (set-property "object-name" "point" (list x y)))))
    (define make-line (lambda (p1 p2) (set-property "thickness" 1 (set-property "object-name" "line" (list p1 p2)))))
    (define make-text (lambda (str) (set-property "text-rotation" 0 (set-property "text-scale" 1 (set-property "position" (define location (make-point 0 0)) (set-property "object-name" "text" (str)))))))
    (define make-label (lambda (str) (set-property "text-rotation" 0 (set-property "text-scale" 1 (set-property "position" (define location (make-point 0 0)) (set-property "object-name" "label" (str)))))))

)
