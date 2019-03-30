open Ctypes
open Protocol

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

let rec worker img conn commandlist =
  match commandlist with
  | [] ->
    let args =
      let p1,p2 = point (), point () in
      let (x1,y1),(x2,y2) = p1,p2 in
      Printf.printf "Line: %i,%i - %i,%i\n" x1 y1 x2 y2;
      flush_all ();
      [Image (CArray.to_list img , (p1,p2)); Display ((p1,p2), Slow); ]
    in

    worker img conn (args)
  | command::xs ->
    match act conn command with
    | Ack | _ -> worker img conn xs

let start commandlist =
  let ic,oc = Unix.open_connection (Unix.ADDR_INET(Unix.inet_addr_any, 1234)) in
  let conn = {ic; oc} in
  (*let () = Unix.descr_of_in_channel ic |> Unix.set_nonblock in*)
  let x,y = match act conn Size with
    | Dim (x,y) -> x,y
    | _ -> Invalid_argument "Size" |> raise
  in
  let img = CArray.make ~initial:0x00 int (x*y) in

  try
    worker img conn commandlist
  with exc -> Printf.fprintf stderr "Exception!"; raise exc
