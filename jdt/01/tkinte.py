import tkinter as tk

def button_click():
    label.config(text="Button Clicked!")

# 创建主窗口
window = tk.Tk()

# 创建标签
label = tk.Label(window, text="Hello World!")
label.pack()

# 创建按钮
button = tk.Button(window, text="Click Me", command=button_click)
button.pack()

# 运行主循环
window.mainloop()
