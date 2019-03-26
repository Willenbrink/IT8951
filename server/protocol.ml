type request =
  | Init
  | Cancel
  | Image
  | Heartbeat
  | Exam1
  | Exam2
  | Exam3

type response =
  | Success
  | Fail
  | Ack
  | Exc of string

let request_to_string = function
  | Init -> "init"
  | Cancel -> "cancel"
  | Image -> "image"
  | Heartbeat -> "heartbeat"
  | _ -> "unknown"

let response_to_string = function
  | Success -> "success"
  | Fail -> "fail"
  | Ack -> "ack"
  | Exc str -> "exc " ^ str

type connection = {ic : in_channel; oc : out_channel}

let send ?(flush = true) oc value =
  try
    Marshal.to_channel oc value [];
    if flush then Pervasives.flush oc
  with Sys_blocked_io | End_of_file -> ()

let recv ic =
  try
    Some (Marshal.from_channel ic)
  with Sys_blocked_io | End_of_file -> None

let recvb ic = Marshal.from_channel ic

let act {ic; oc} value =
  Marshal.to_channel oc value [];
  flush oc;
  let rec value () =
    try
      Marshal.from_channel ic
    with End_of_file -> raise End_of_file; Unix.sleep 1; value ()
  in
  value ()

let react {ic; oc} handle =
  let request = Marshal.from_channel ic in
  let response = handle request in
  Marshal.to_channel oc response [];
  flush oc;
  (request, response)




