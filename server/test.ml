open Protocol

let main () =
  match Unix.fork () with
  | 0 -> Server.start ()
  | _ -> Unix.sleep 1; Client.start [Init; Exam1; Exam2; Exam3]

;;main ()
