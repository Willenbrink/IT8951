open Interface
open Protocol

let init addr = Unix.open_connection addr

let random int = Random.int int

let point () = random 799, random 599

let char () = random 255 |> Char.chr

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
    let args =
      let p1,p2 = point (), point () in
      let (x1,y1),(x2,y2) = p1,p2 in
      Printf.printf "Line: %i,%i - %i,%i\n" x1 y1 x2 y2;
      flush_all ();
      [draw_line p1 p2; display (p1) (p2); ]
    in

    worker conn (args)
  | command::xs ->
    match act conn command with
    | None -> worker conn (command::xs)
    | Some _ -> worker conn xs

let start commandlist =
  let ic,oc = init (Unix.ADDR_INET(Unix.inet_addr_any, 1234)) in
  let conn = {ic; oc} in
  (*let () = Unix.descr_of_in_channel ic |> Unix.set_nonblock in*)

  try
    act conn Interface.init |> ignore;
    act conn clear |> ignore;
    act conn display_all |> ignore;
    worker conn commandlist;
    (match act conn free with
     | res -> ())

  with exc -> Printf.fprintf stderr "Exception! "; raise exc
