"""
ecualizador_gui.py
==================
GUI de ecualizador para LPC1769 — 3 bandas (Bajo / Medio / Alto)

Protocolo UART (115200 8N1):
  RECEPCIÓN  (PC → LPC): 1 byte → ganancia banda activa [0x00..0xFF]
  TRANSMISIÓN (LPC → PC): 1 byte de selección de banda
    0x10 → BANDA BAJA   (teclado P0.0)
    0x11 → BANDA MEDIA  (teclado P0.1)
    0x12 → BANDA ALTA   (teclado P0.2)
"""

import tkinter as tk
from tkinter import ttk, messagebox
import threading
import queue
import time
import serial
import serial.tools.list_ports

# ─────────────────────────────────────────────
# Protocolo
# ─────────────────────────────────────────────
SEL_BAJOS  = 0x10
SEL_MEDIOS = 0x11
SEL_AGUDOS = 0x12

BANDA_MAP  = {SEL_BAJOS: 0, SEL_MEDIOS: 1, SEL_AGUDOS: 2}
BAND_NAMES = ["BAJO", "MEDIO", "ALTO"]
BAUD_RATE  = 115200

# ─────────────────────────────────────────────
# Paleta
# ─────────────────────────────────────────────
BG         = "#0D0D0D"
PANEL_BG   = "#141414"
ACCENT     = "#FF6B00"
ACCENT_DIM = "#7A3300"
SLIDER_ACT = "#FF8C38"
SLIDER_LOK = "#3A3A3A"
TRACK_LOK  = "#2A2A2A"
TEXT_ACT   = "#FFFFFF"
TEXT_LOK   = "#555555"
IND_COL    = "#FF6B00"

FN_LABEL  = ("Courier New", 10, "bold")
FN_VALUE  = ("Courier New",  9)
FN_STATUS = ("Courier New",  8)
FN_SIM    = ("Courier New",  9, "bold")


# ══════════════════════════════════════════════
# BandSlider
# ══════════════════════════════════════════════
class BandSlider(tk.Canvas):
    TRACK_W = 4
    KNOB_R  = 14
    PAD     = 30

    def __init__(self, parent, label, on_change=None, **kw):
        super().__init__(parent, bg=PANEL_BG, highlightthickness=0,
                         width=80, height=300, **kw)
        self.label     = label
        self.on_change = on_change
        self._value    = 0.5
        self._locked   = True
        self._drag     = False

        self.bind("<ButtonPress-1>",   self._press)
        self.bind("<B1-Motion>",       self._drag_cb)
        self.bind("<ButtonRelease-1>", self._release)
        self.bind("<Configure>",       lambda e: self._draw())
        self._draw()

    @property
    def value(self):
        return self._value

    @value.setter
    def value(self, v):
        self._value = max(0.0, min(1.0, v))
        self._draw()

    @property
    def locked(self):
        return self._locked

    @locked.setter
    def locked(self, f):
        self._locked = f
        self._draw()

    def _draw(self):
        self.delete("all")
        w  = self.winfo_width()  or 80
        h  = self.winfo_height() or 300
        cx = w // 2
        tt = self.PAD
        tb = h - self.PAD
        th = tb - tt

        if self._locked:
            tc, kf, ko = TRACK_LOK, SLIDER_LOK, "#555555"
            lc, vc     = TEXT_LOK, TEXT_LOK
        else:
            tc, kf, ko = ACCENT_DIM, SLIDER_ACT, ACCENT
            lc, vc     = TEXT_ACT, ACCENT

        self.create_line(cx, tt, cx, tb, fill=tc,
                         width=self.TRACK_W, capstyle=tk.ROUND)

        ky = tt + (1.0 - self._value) * th
        r  = self.KNOB_R

        if not self._locked:
            self.create_oval(cx-r+2, ky-r+2, cx+r+2, ky+r+2,
                             fill="#1A1A1A", outline="")

        self.create_oval(cx-r, ky-r, cx+r, ky+r,
                         fill=kf, outline=ko,
                         width=2 if not self._locked else 1)
        self.create_line(cx-r+6, ky, cx+r-6, ky,
                         fill=ko if not self._locked else "#444",
                         width=2, capstyle=tk.ROUND)

        pct = int(self._value * 100)
        self.create_text(cx, ky-r-8, text=f"{pct}%",
                         fill=vc, font=FN_VALUE, anchor="s")
        self.create_text(cx, h-8, text=self.label,
                         fill=lc, font=FN_LABEL, anchor="s")

        if not self._locked:
            self.create_oval(cx-4, 10, cx+4, 18, fill=IND_COL, outline="")

    def _y2v(self, y):
        h = self.winfo_height() or 300
        return 1.0 - max(0.0, min(1.0, (y - self.PAD) / max(1, h - 2*self.PAD)))

    def _press(self, e):
        if not self._locked:
            self._drag = True

    def _drag_cb(self, e):
        if self._locked or not self._drag:
            return
        self._value = self._y2v(e.y)
        self._draw()
        if self.on_change:
            self.on_change(self._value)

    def _release(self, e):
        self._drag = False


# ══════════════════════════════════════════════
# Log UART
# ══════════════════════════════════════════════
class UartLog(tk.Frame):
    MAX_LINES = 60

    def __init__(self, parent, **kw):
        super().__init__(parent, bg=BG, **kw)
        tk.Label(self, text="LOG UART", bg=BG, fg="#444",
                 font=FN_SIM).pack(anchor="w", padx=6)
        self._txt = tk.Text(self, bg="#0A0A0A", fg="#555",
                            font=("Courier New", 7),
                            relief="flat", height=6, width=52,
                            state="disabled", wrap="none")
        self._txt.pack(padx=6, pady=(2, 6), fill="x")

    def append(self, msg: str):
        ts = time.strftime("%H:%M:%S")
        self._txt.config(state="normal")
        self._txt.insert("end", f"[{ts}] {msg}\n")
        lines = int(self._txt.index("end-1c").split(".")[0])
        if lines > self.MAX_LINES:
            self._txt.delete("1.0", f"{lines - self.MAX_LINES}.0")
        self._txt.see("end")
        self._txt.config(state="disabled")


# ══════════════════════════════════════════════
# Ventana principal
# ══════════════════════════════════════════════
class EqApp(tk.Tk):

    def __init__(self):
        super().__init__()
        self.title("Ecualizador LPC1769")
        self.resizable(False, False)
        self.configure(bg=BG)

        self._active_band = 0
        self._gains       = [0.5, 0.5, 0.5]
        self._serial      = None
        self._running     = False
        self._rx_q        = queue.Queue()

        self._build_ui()
        self._refresh_sliders()
        self._start_queue_consumer()
        self.protocol("WM_DELETE_WINDOW", self._on_close)

    def _build_ui(self):
        # Título
        hdr = tk.Frame(self, bg=BG, pady=10)
        hdr.pack(fill="x", padx=20)
        tk.Label(hdr, text="█ ECUALIZADOR", bg=BG, fg=ACCENT,
                 font=("Courier New", 16, "bold")).pack(side="left")
        tk.Label(hdr, text="LPC1769", bg=BG, fg="#444",
                 font=("Courier New", 16)).pack(side="left", padx=6)

        # Panel conexión
        conn = tk.Frame(self, bg=PANEL_BG, padx=12, pady=8)
        conn.pack(fill="x", padx=20, pady=(0, 10))

        tk.Label(conn, text="PUERTO:", bg=PANEL_BG, fg="#888",
                 font=FN_STATUS).grid(row=0, column=0, sticky="w")

        self._port_var = tk.StringVar()
        self._port_cb  = ttk.Combobox(conn, textvariable=self._port_var,
                                       width=14, font=FN_STATUS, state="readonly")
        self._port_cb.grid(row=0, column=1, padx=6)
        self._refresh_ports()

        self._btn_conn = tk.Button(conn, text="CONECTAR",
                                    bg=ACCENT, fg="#000",
                                    font=FN_STATUS, relief="flat",
                                    padx=10, cursor="hand2",
                                    command=self._toggle_connect)
        self._btn_conn.grid(row=0, column=2, padx=4)

        tk.Button(conn, text="↺", bg=PANEL_BG, fg="#888",
                  font=FN_STATUS, relief="flat", cursor="hand2",
                  command=self._refresh_ports).grid(row=0, column=3)

        # Indicador banda activa
        ind = tk.Frame(self, bg=BG, pady=4)
        ind.pack()
        tk.Label(ind, text="BANDA ACTIVA:", bg=BG, fg="#666",
                 font=FN_STATUS).pack(side="left")
        self._band_lbl = tk.Label(ind, text="—", bg=BG, fg="#555",
                                   font=("Courier New", 11, "bold"))
        self._band_lbl.pack(side="left", padx=6)

        # Sliders
        sf = tk.Frame(self, bg=PANEL_BG, padx=20, pady=20)
        sf.pack(padx=20, pady=4)
        self._sliders = []
        for i, name in enumerate(BAND_NAMES):
            s = BandSlider(sf, name,
                           on_change=lambda v, idx=i: self._on_slider(idx, v))
            s.pack(side="left", padx=18)
            self._sliders.append(s)

        # Log
        self._log = UartLog(self)
        self._log.pack(fill="x", padx=20, pady=(4, 0))

        # Barra de estado
        self._status_var = tk.StringVar(value="Sin conexión — seleccioná el puerto COM del LPC1769")
        tk.Label(self, textvariable=self._status_var,
                 bg="#0A0A0A", fg="#555", font=FN_STATUS,
                 anchor="w", padx=12, pady=4).pack(fill="x", side="bottom")

    def _refresh_ports(self):
        ports = [p.device for p in serial.tools.list_ports.comports()]
        self._port_cb["values"] = ports
        if ports:
            self._port_var.set(ports[0])

    def _toggle_connect(self):
        if self._serial and self._serial.is_open:
            self._disconnect()
        else:
            self._connect()

    def _connect(self):
        port = self._port_var.get()
        if not port:
            messagebox.showwarning("Sin puerto", "Seleccioná un puerto COM.")
            return
        try:
            self._serial  = serial.Serial(port, BAUD_RATE, timeout=0.1)
            self._running = True
            threading.Thread(target=self._rx_loop, daemon=True).start()
            self._btn_conn.config(text="DESCONECTAR", bg="#C0392B")
            self._status_var.set(f"Conectado en {port} — {BAUD_RATE} 8N1  |  Presioná un botón en el LPC")
            self._log.append(f"CONECTADO {port} @ {BAUD_RATE}")
        except serial.SerialException as e:
            messagebox.showerror("Error de puerto", str(e))

    def _disconnect(self):
        self._running = False
        if self._serial:
            self._serial.close()
            self._serial = None
        self._btn_conn.config(text="CONECTAR", bg=ACCENT)
        self._band_lbl.config(text="—", fg="#555")
        self._status_var.set("Desconectado")
        self._log.append("DESCONECTADO")
        # Bloquear todos los sliders
        for s in self._sliders:
            s.locked = True

    def _rx_loop(self):
        while self._running and self._serial and self._serial.is_open:
            try:
                raw = self._serial.read(1)
                if raw:
                    self._rx_q.put(raw[0])
            except Exception:
                break

    def _start_queue_consumer(self):
        def _poll():
            try:
                while True:
                    byte = self._rx_q.get_nowait()
                    self._process_rx(byte)
            except queue.Empty:
                pass
            self.after(30, _poll)
        self.after(30, _poll)

    def _process_rx(self, byte: int):
        if byte in BANDA_MAP:
            idx = BANDA_MAP[byte]
            self._log.append(f"RX  ← 0x{byte:02X}  →  banda {BAND_NAMES[idx]}")
            self._set_active_band(idx)
        else:
            self._log.append(f"RX  ← 0x{byte:02X}  (desconocido)")

    def _set_active_band(self, idx: int):
        self._active_band = idx
        self._band_lbl.config(text=BAND_NAMES[idx], fg=ACCENT)
        self._status_var.set(f"Banda activa: {BAND_NAMES[idx]}  —  mové el slider para ajustar la ganancia")
        self._refresh_sliders()

    def _refresh_sliders(self):
        for i, s in enumerate(self._sliders):
            s.locked = (i != self._active_band)
            s.value  = self._gains[i]

    def _on_slider(self, idx: int, value: float):
        if idx != self._active_band:
            return
        self._gains[idx] = value
        byte_val = int(value * 255)
        self._send_byte(byte_val)
        self._log.append(f"TX  → 0x{byte_val:02X}  ({int(value*100):3d}%)  [{BAND_NAMES[idx]}]")
        self._status_var.set(f"→ Ganancia {BAND_NAMES[idx]}: {int(value*100)}%  (0x{byte_val:02X})")

    def _send_byte(self, val: int):
        if self._serial and self._serial.is_open:
            try:
                self._serial.write(bytes([val & 0xFF]))
            except Exception as e:
                self._log.append(f"ERROR TX: {e}")

    def _on_close(self):
        self._disconnect()
        self.destroy()


if __name__ == "__main__":
    app = EqApp()
    app.mainloop()
