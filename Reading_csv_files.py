import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import os
from tkinter import *
from tkinter import filedialog
from tkinter import ttk  

# Define the directory path
#directory_path = "/Volumes/RECORDINGS"  # Ensure no trailing backslash

# Use glob to find all CSV files in the directory
#csv_files = glob.glob(os.path.join(directory_path, "*.csv"))

# Check if any CSV files were found
#if not csv_files:
#    print(f"No CSV files found in {directory_path}")
#    exit()





#label_file_explorer = Label(window, 
#                            text = "File Explorer using Tkinter",
#                            width = 100, height = 4, 
#                            fg = "blue")
#label_file_explorer.grid(column=1, row=1)

#define global variables

global_file_path = ""
stats_labels = []
combo_x = None
combo_y = None 


#define functions

def printFileData():
    try:
        # Read the CSV file
        data = pd.read_csv(global_file_path)
        file_name = os.path.basename(global_file_path)  # Extract the file name

        # Plot the data
        plt.figure(figsize=(15, 5))

        # Distance vs Time
        plt.subplot(1, 3, 1)
        plt.plot(data["Time"], data["Distance"], marker='o', linestyle='-', color='b')
        plt.title(f'Distance vs Time ({file_name})')
        plt.xlabel('Time')
        plt.ylabel('Distance')
        plt.grid(True)

        # Velocity vs Time
        plt.subplot(1, 3, 2)
        plt.plot(data["Time"], data["Velocity"], marker='o', linestyle='-', color='r')
        plt.title(f'Velocity vs Time ({file_name})')
        plt.xlabel('Time')
        plt.ylabel('Velocity')
        plt.grid(True)

        # Acceleration vs Time
        plt.subplot(1, 3, 3)
        plt.plot(data["Time"], data["Acceleration"], marker='o', linestyle='-', color='g')
        plt.title(f'Acceleration vs Time ({file_name})')
        plt.xlabel('Time')
        plt.ylabel('Acceleration')
        plt.grid(True)

        plt.tight_layout()
        plt.show()

    except Exception as e:
        print(f"Error processing {global_file_path}: {e}")
    return


def printPlot():
    global combo_x
    global combo_y
    x_data = combo_x.get()
    y_data = combo_y.get()


    data = pd.read_csv(global_file_path)
    file_name = os.path.basename(global_file_path)  # Extract the file name

    # Plot the data
    #plt.figure(figsize=(15, 5))
    plt.figure()
    plt.plot(data[x_data], data[y_data], marker='o', linestyle='-', color='b')
    plt.title(f'{x_data} vs {y_data} ({file_name})')
    plt.xlabel(x_data)
    plt.ylabel(y_data)
    plt.show()



def openFile():
    global global_file_path
    filename = filedialog.askopenfilename(initialdir = "/", title = "Select a csv File",
                                          filetypes = [("CSV files",
                                                        "*.csv")])
    if filename:
        file_label.config(text=f"Selected File: {os.path.basename(filename)}")  # Update label
        global_file_path = filename
        #printFileData(filename)
    showStats()
    showDropdowns()

def showStats():
    global stats_labels
    data = pd.read_csv(global_file_path)

    #get total time
    total_time = data["Time"].iloc[-1]

    #get total distance
    total_distance = data["Distance"].iloc[-1]

    #get average velocity
    average_velocity = data["Velocity"].mean()

    #get average stroke rate
    average_stroke_rate = data["Stroke_Rate"].mean()

    #clear previous stats
    for label in stats_labels:
        label.destroy()
    stats_labels = []

     # Create new labels for the statistics
    stats = [
        f"Total Time: {total_time} s",
        f"Total Distance: {total_distance} m",
        f"Average Velocity: {average_velocity:.2f} m/s",
        f"Average Stroke Rate: {average_stroke_rate:.2f} strokes/min"
    ]

    # Display labels in the GUI
    for i, stat in enumerate(stats):
        label = Label(frame, text=stat, bg=macos_bg, fg="black")
        label.grid(column=2, row=0 + i, pady=5)  # Position below existing elements
        stats_labels.append(label)  # Store label references

    

def showDropdowns():
    """Function to display dropdowns after selecting a file"""
    global combo_x
    global combo_y
    columns = ["Time", "Distance", "Velocity", "Stroke_Rate", "Acceleration"]
    # Remove previous dropdowns if they exist
    for widget in frame.winfo_children():
        if isinstance(widget, ttk.Combobox):
            widget.destroy()
    
    # Label for first dropdown
    label1 = Label(frame, text="Select X-axis:", bg=macos_bg, fg="black")
    label1.grid(column=1, row=0, pady=5)
    
    # First dropdown (X-axis selection)
    combo_x = ttk.Combobox(frame, values=columns, state="readonly")
    combo_x.grid(column=1, row=1, pady=5)
    combo_x.current(0)  # Set default selection

    # Label for second dropdown
    label2 = Label(frame, text="Select Y-axis:", bg=macos_bg, fg="black")
    label2.grid(column=1, row=2, pady=5)
    
    # Second dropdown (Y-axis selection)
    combo_y = ttk.Combobox(frame, values=columns, state="readonly")
    combo_y.grid(column=1, row=3, pady=5)
    combo_y.current(1 if len(columns) > 1 else 0)  # Set default selection

    #plot button
    button_explore = ttk.Button(frame, 
                        text = "Plot",
                        command = printPlot) 
    button_explore.grid(column=1, row=4, pady=10)


window = Tk()
window.title('Row Buddy Viewer')
window.geometry("800x500")
window.config(background = "white")

# Match the window background to macOS system color
macos_bg = "#F0F0F0"  # Light gray macOS background
window.config(background=macos_bg)

# Apply macOS "aqua" style
style = ttk.Style()
style.theme_use("aqua")  # Ensures macOS styling is used
style.configure("TButton",
                background=macos_bg,  # Match window color
                borderwidth=0,  # No extra border
                relief="flat")
      
#centre window
window.columnconfigure(0, weight=1)
window.rowconfigure(0, weight=1)

#make a frame
frame = Frame(window, bg="#F0F0F0")
frame.grid(row=0, column=0)


# Label to display file name
file_label = Label(frame, text="Select file", bg=macos_bg, fg="black")
file_label.grid(column=0, row=0, pady=10)

button_explore = ttk.Button(frame, 
                        text = "Open csv file",
                        command = openFile) 
button_explore.grid(column=0, row=1, pady=10)

  
button_exit = ttk.Button(frame, 
                     text = "Exit",
                     command = exit) 

button_exit.grid(column=0, row=2, pady=10)



window.mainloop()




