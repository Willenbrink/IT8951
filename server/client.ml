open Protocol

let init addr = Unix.open_connection addr

let rec worker conn commandlist =
  match commandlist with
  | [] ->
    (match act conn Heartbeat with
     | res ->
       print_string "Alive: ";
       response_to_string res |> print_endline
    );
    Unix.sleep 1;
    worker conn []
  | command::xs ->
    request_to_string command |> print_endline;
    act conn command |> response_to_string |> print_endline;
    worker conn xs


let start commandlist =
  let ic,oc = init (Unix.ADDR_INET(Unix.inet_addr_any, 1234)) in
  (*let () = Unix.descr_of_in_channel ic |> Unix.set_nonblock in*)

  try
    worker {ic; oc} commandlist
  with exc -> Printf.fprintf stderr "Exception! "; raise exc
