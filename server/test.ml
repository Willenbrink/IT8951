open Protocol

let arguments =
  [
    Init;
    Exam2;
  ]

let main () =
  if Array.length Sys.argv <> 1 then
    if Sys.argv.(1) = "1" then
      Server.start ()
    else
      Client.start arguments
  else
  match Unix.fork () with
  | 0 -> Server.start ()
  | _ -> Unix.sleep 1; Client.start arguments

;;main ()
