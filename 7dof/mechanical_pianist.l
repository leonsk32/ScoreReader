(load "package://dynamixel_7dof_arm/euslisp/dxl-7dof-arm-interface.l")
(dxl-7dof-arm-init)

(setq calib (open "result_calib.dat" :direction :input))
(setq index nil)
(while t
  (setq pitch (read-line calib nil nil))
  (if (not pitch)
      (progn
	(close calib)
	(return-from nil))
    nil)
  (setq a0 (read-from-string (read-line calib nil nil)))
  (setq a1 (read-from-string (read-line calib nil nil)))
  (setq a2 (read-from-string (read-line calib nil nil)))
  (setq a3 (read-from-string (read-line calib nil nil)))
  (setq a4 (read-from-string (read-line calib nil nil)))
  (setq a5 (read-from-string (read-line calib nil nil)))
  (setq angle (float-vector a0 a1 a2 a3 a4 a5 0))
  (setq pitch_set (cons (read-from-string pitch) angle))
  (setq index (cons pitch_set index)))

(setq def-angle (cdr (assoc 'DEF index)))
;(send *dxl-7dof-arm* :init-pose)
(send *dxl-7dof-arm* :angle-vector def-angle)
(send *dxl-7dof-arm* :gripper :angle-vector #f(0))
(send *ri* :angle-vector (send *dxl-7dof-arm* :angle-vector) 2000)
(send *ri* :angle-vector (send *dxl-7dof-arm* :angle-vector) 500 :gripper-controller)
(send *ri* :wait-interpolation)

(setq score (open "lisp_score_data.dat" :direction :input))
(while t
  (setq length_c (read-line score nil nil))
  (if (not length_c)
      (progn
	(warn "finish.~%")
	(close score)
	(return-from nil))
    nil)
  (setq length (read-from-string length_c))
  (if (> length 0)
      (progn
	(setq pitch (read-line score nil nil))
	(setq angle (cdr (assoc (read-from-string pitch) index))))
    (progn
      (setq angle def-angle)
      (setq length (- length))))
  (send *dxl-7dof-arm* :angle-vector angle)
  (send *ri* :angle-vector (send *dxl-7dof-arm* :angle-vector) 1000)
  (send *ri* :wait-interpolation)
  (unix::usleep (* (- length 1) 1700000))
  (send *dxl-7dof-arm* :angle-vector def-angle)
  (send *ri* :angle-vector (send *dxl-7dof-arm* :angle-vector) 700)
  (send *ri* :wait-interpolation))
(send *ri* :servo-off-all)

#|
リストの定義は値や文字なら(setq a '(1 2))、評価の必要があれば(setq a (list <atom0> <atom1>...))
連想リストなら(setq a '(1 . 2))、(setq a (cons <atom0> <atom1>))
連想リストのキーは数値または変数名でなければならない模様(文字列では検索できなかった)
(cons <atom0> (list <atom1> <atom2> ...)) = (<atom0> <atom1> <atom2> ...) ...要素付け足し
(append (list <atom1> <atom2>) (list <atom3> <atom4>)) = (<atom1> <atom2> <atom3> <atom4>) ...リスト連結
シンボルを"そのまま"にしたければ"'"をつける。
ex)
(equal (+ 3 5) 8) ...t
(equal '(+ 3 5) 8) ...nil '(+ 3 5)は(+ 3 5)という単語
(equal (+ 3 5) '8) ...t 値はもともとそのまま
(setq a (list 1 2)) ...(1 2)
(setq b (list 1 2)) ...(1 2)
(equal a b) ...t
(equal 'a b) ...nil 'aはaという単語 bは(1 2)というリスト
(equal 'a 'b) ...nil シンボルは同じではない
(equal 'a (read-from-string "a")) ...t read-from-stringは引数が数値なら値に、それ以外なら変数名にする。
(setq c 'x) ...x
c ...x
'c ...c
(equal c x) ...error xという変数は宣言されていないので評価しようがない
(setq x 5) ...5
(equal c x) ...nil
(equal c 'x) ...t
|#