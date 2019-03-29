type request =
  | Init
  | Free
  | Clear
  | Display
  | Plot of int * int
  | Color_point of int * int
  | Line of int * int * int * int
  | Color_set of int
  | Image
  | Heartbeat
  | Exam1
  | Exam2
  | Exam3

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
