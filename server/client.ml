open Interface
open Protocol

let init addr = Unix.open_connection addr

let random int = Random.int int

let line () = plot (random 800, random 600)

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
    let args = linit 1000 (fun _ -> line ()) in
    worker conn (display::args)
  | command::xs ->
    match act conn command with
    | None -> worker conn (command::xs)
    | Some x -> worker conn xs

let start commandlist =
  let ic,oc = init (Unix.ADDR_INET(Unix.inet_addr_any, 1234)) in
  let conn = {ic; oc} in
  (*let () = Unix.descr_of_in_channel ic |> Unix.set_nonblock in*)

  try
    act conn Interface.init |> ignore;
    act conn clear |> ignore;
    worker conn commandlist;
    (match act conn free with
     | res -> ())

  with exc -> Printf.fprintf stderr "Exception! "; raise exc
