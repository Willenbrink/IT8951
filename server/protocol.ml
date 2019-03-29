type connection = {ic : in_channel; oc : out_channel}

let act {ic; oc} (f : unit -> 'a) =
  Marshal.to_channel oc f [Marshal.Closures];
  flush oc;
  ((Marshal.from_channel ic) : 'a option)

let react {ic; oc} =
  let request : (unit -> 'a) = Marshal.from_channel ic in
  let response =
    try Some (request ())
    with _ -> None
  in
  Marshal.to_channel oc (response : 'a option) [Marshal.Closures];
  flush oc
