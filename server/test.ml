open Protocol

let arguments =
  [
  ]

let main () =
  if Array.length Sys.argv <> 1 then
    if Sys.argv.(1) = "1" then
      Networkify.start ()
    else
      Client.start arguments
  else
  match Unix.fork () with
  | 0 -> Networkify.start ()
  | _ -> Unix.sleep 1; Client.start arguments

;;main ()
