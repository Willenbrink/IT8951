open Protocol

let init addr = Unix.open_connection addr

let random int = Random.int int

let line () = Line((random 800, random 600), (random 800, random 600))

let rec init_aux i n f =
  if i >= n then []
  else
    let r = f i in
    r :: init_aux (i+1) n f

let linit len f =
  if len < 0 then invalid_arg "List.init" else
    init_aux 0 len f

let rec worker conn commandlist =
  match commandlist with
  | [] ->
    let args = linit 10 (fun _ -> line ()) in
    worker conn (Display::args)
  | command::xs ->
    request_to_string command |> fun s -> s ^ ": " |> print_string;
    act conn command |> response_to_string |> print_endline;
    worker conn xs

let start commandlist =
  let ic,oc = init (Unix.ADDR_INET(Unix.inet_addr_any, 1234)) in
  let conn = {ic; oc} in
  (*let () = Unix.descr_of_in_channel ic |> Unix.set_nonblock in*)

  try
    act conn Init |> ignore;
    act conn Clear |> ignore;
    worker conn commandlist;
    (match act conn Free with
     | res ->
       print_string "Exit: ";
       response_to_string res |> print_endline)

  with exc -> Printf.fprintf stderr "Exception! "; raise exc
