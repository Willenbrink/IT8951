open Interface

type request =
  | Init
  | Free
  | Heartbeat
  | Clear
  | Display
  | Point of point
  | Color_at_point of point
  | Line of point * point
  | Color_set of int
  | Image

type response =
  | Success
  | Fail
  | Color of int
  | Ack
  | Exc of string

let request_to_string = function
  | Init -> "init"
  | Free -> "free"
  | Image -> "image"
  | Heartbeat -> "heartbeat"
  | _ -> "unknown"

let response_to_string = function
  | Success -> "success"
  | Fail -> "fail"
  | Ack -> "ack"
  | Exc str -> "exc " ^ str
  | _ -> "unknown response"

type connection = {ic : in_channel; oc : out_channel}

let act {ic; oc} value =
  Marshal.to_channel oc value [];
  flush oc;
  Marshal.from_channel ic

let react {ic; oc} handle =
  let request = Marshal.from_channel ic in
  let response = handle request in
  Marshal.to_channel oc response [];
  flush oc;
  (request, response)
