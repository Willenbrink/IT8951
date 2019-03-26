type request =
  | Init
  | Cancel
  | Image

type response =
  | Ack
  | Fail

let send ?(flush = true) oc value =
  try
    Marshal.to_channel oc value [];
    if flush then Pervasives.flush oc
  with Sys_blocked_io -> ()

let recv ic =
  try
    Some (Marshal.from_channel ic)
  with Sys_blocked_io -> None

