let auto_synchronize _ = ()
let display_mode _ = ()
let remember_mode _ = ()

let foreground = EPD.fg
let background = EPD.bg

let pos_x = ref 0
let pos_y = ref 0

let incx x = pos_x := !pos_x + x
let incy x = pos_x := !pos_y + x

let open_graph _ = EPD.init () |> ignore

let set_color c = EPD.set_color c

let plot x y = print_endline "Plotting: "; EPD.plot (x,y)

let set_line_width _ = ()

let set_text_size _ = ()

let draw_char c = EPD.draw_text (!pos_x, !pos_y) (String.make 1 c); incx 8; incy 8

let draw_string c = EPD.draw_text (!pos_x, !pos_y) c; incx 8; incy 8

let moveto x y = pos_x := x; pos_y := y

let rmoveto x y = incx x; incy y

let lineto x y = EPD.draw_line (!pos_x,!pos_y) (x,y); incx x; incy y
let rlineto x y = EPD.draw_line (!pos_x,!pos_y) ((!pos_x + x),(!pos_y + y)); incx x; incy y

let draw_circle x y r = EPD.draw_circle (x,y) r
let fill_circle x y r = EPD.fill_circle (x,y) r

let draw_rect x y a b = EPD.draw_rect (x,y) (a,b)
let fill_rect x y a b = EPD.fill_rect (x,y) (a,b)

let draw_arc x y rx ry _ _ = draw_circle x y (rx + ry)
let fill_arc x y rx ry _ _ = fill_circle x y (rx + ry)

let draw_poly _ = ()
let fill_poly _ = ()

let draw_ellipse x y rx ry = draw_circle x y (rx + ry)
let fill_ellipse x y rx ry = fill_circle x y (rx + ry)

let synchronize () = EPD.display_buffer (0,0) (800, 600)

let clear_graph () = EPD.clear background

let current_point () = !pos_x,!pos_y

let rgb = EPD.rgb

let white = 0xF0
let transp = 0xF0
let yellow = 0xD0
let blue = 0xB0
let cyan = 0x90
let green = 0x70
let red = 0x50
let black = 0x00

