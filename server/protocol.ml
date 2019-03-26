type request =
  | Init
  | Cancel
  | Image
  | Heartbeat

type response =
  | Ack
  | Fail
  | Exc

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

