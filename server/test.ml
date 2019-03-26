open Protocol

let main () =
  match Unix.fork () with
  | 0 -> Server.start ()
  | _ -> Unix.sleep 1; Client.start [Heartbeat; Heartbeat; Heartbeat; Heartbeat; Init]

;;main ()
