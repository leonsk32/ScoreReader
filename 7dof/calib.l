(load "package://dynamixel_7dof_arm/euslisp/dxl-7dof-arm-interface.l")
(dxl-7dof-arm-init)

(send *dxl-7dof-arm* :init-pose)
(send *dxl-7dof-arm* :gripper :angle-vector #f(0))
(send *ri* :angle-vector (send *dxl-7dof-arm* :angle-vector) 2000)
(send *ri* :angle-vector (send *dxl-7dof-arm* :angle-vector) 500 :gripper-controller)
(send *ri* :wait-interpolation)

(send *ri* :servo-off 1)
(send *ri* :servo-off 2)
(send *ri* :servo-off 3)
(send *ri* :servo-off 4)
(send *ri* :servo-off 5)
(send *ri* :servo-off 6)

(warn "calibration start. mode?(o:overwrite a:append) :")
(setq mode (read-line))
(setq index nil);連想(コンス)リスト
(if (not (equal mode "o"));追加モードなら過去のデータを読み込み
    (progn
      (setq fp (open "result_calib.dat" :direction :input))
      (while t
	(setq pitch (read-line fp nil nil))
	(if (not pitch)
	    (progn
	      (close fp)
	      (return-from nil));データがなくなったら終わり
	  nil);データがあればそのまま
	(setq a0 (read-from-string (read-line fp nil nil)))
	(setq a1 (read-from-string (read-line fp nil nil)))
	(setq a2 (read-from-string (read-line fp nil nil)))
	(setq a3 (read-from-string (read-line fp nil nil)))
	(setq a4 (read-from-string (read-line fp nil nil)))
	(setq a5 (read-from-string (read-line fp nil nil)))
	(setq angle (float-vector a0 a1 a2 a3 a4 a5 0))
	(setq pitch_set (cons (read-from-string pitch) angle))
	(setq index (cons pitch_set index)))))

(warn "(e:end)~%")
(while t
  (setq pitch (read-line))
  (cond
   ((equal pitch "e") (return-from nil));endが入力されたら終了
   ((not pitch));何も入れられなければ何もしない
   (t (progn
	(setq angle (send *ri* :state :potentio-vector))
	(setq pitch_set (cons (read-from-string pitch) angle));新しい連想リスト要素
	(setq old (assoc (read-from-string pitch) index));過去に定義されているか
	(if (not old)
	    (progn
	      (warn "new      ")
	      (setq index (cons pitch_set index)));未定義なら追加
	  (progn
	    (warn "improved ")
	    (setq index (subst pitch_set old index))));過去に定義されていれば新しいものに置換
	(format t "~A~%" pitch_set)))))

(setq fp (open "result_calib.dat" :direction :output))
(while t
  (if (not index)
      (progn
	(close fp)
	(return-from nil))
    (progn
      (setq pitch (car (car index)))
      (format fp "~A~%" pitch)
      (setq angle (cdr (car index)))
      (format fp "~A~%" (elt angle 0))
      (format fp "~A~%" (elt angle 1))
      (format fp "~A~%" (elt angle 2))
      (format fp "~A~%" (elt angle 3))
      (format fp "~A~%" (elt angle 4))
      (format fp "~A~%" (elt angle 5))
      (setq index (cdr index)))))
   
(warn "finish.~%")
(send *ri* :servo-off-all)