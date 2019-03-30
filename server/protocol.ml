open Ctypes

type point = int * int
type area = point * point
type image = int carray
type mode = White | Unknown | Slow | Medium | Fast

type request =
  | Display of area * mode
  | Image of int list * area
  | Size

type response =
  | Dim of point
  | Ack

let request_to_string = function
  | Display _ -> "display"
  | Image _ -> "image"
  | Size -> "size"

type connection = {ic : in_channel; oc : out_channel}

let act {ic; oc} value =
  Marshal.to_channel oc value [];
  flush oc;
  (Marshal.from_channel ic : response)

let react {ic; oc} handle =
  let request = (Marshal.from_channel ic : request) in
  (*print_endline ("Do something: " ^ (request_to_string request));*)
  let response = handle request in
  Marshal.to_channel oc response [];
  flush oc
