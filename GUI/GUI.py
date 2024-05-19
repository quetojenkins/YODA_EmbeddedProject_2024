from PyQt5.QtWidgets import QApplication, QWidget, QVBoxLayout, QLabel, QPushButton, QLineEdit, QHBoxLayout, QSpacerItem, QSizePolicy, QMessageBox,QFileDialog
from PyQt5.QtGui import QPixmap, QColor, QFont
from PyQt5.QtCore import Qt
import sys,subprocess
from PyQt5.QtGui import QIcon



global text_box

global self
self = None

global passworddictionarypath
passworddictionarypath = ""

global personalinfopath
personalinfopath = ""

global hashespath
hashespath = ""

def button_clicked1():
    # Function for button 1
    options = QFileDialog.Options()
    options |= QFileDialog.Option.ShowDirsOnly  # Restrict to directories only (optional)
    file_path, _ = QFileDialog.getOpenFileName(self, "Select Hashes File", "", "All Files (*);;Text Files (*.txt)", options=options)
    if file_path:
        print(f"Selected file: {file_path}")  # Print the chosen file path
        global hashespath
        hashespath = file_path


def button_clicked2():
    # Function for button 1
    options = QFileDialog.Options()
    options |= QFileDialog.Option.ShowDirsOnly  # Restrict to directories only (optional)
    file_path, _ = QFileDialog.getOpenFileName(self, "Select Personal Info File", "", "All Files (*);;Text Files (*.txt)", options=options)
    if file_path:
        print(f"Selected file: {file_path}")  # Print the chosen file path
        global personalinfopath
        personalinfopath = file_path


def button_clicked3():
    # Function for button 1
    options = QFileDialog.Options()
    options |= QFileDialog.Option.ShowDirsOnly  # Restrict to directories only (optional)
    file_path, _ = QFileDialog.getOpenFileName(self, "Select Dictionary File", "", "All Files (*);;Text Files (*.txt)", options=options)
    if file_path:
        print(f"Selected file: {file_path}")  # Print the chosen file path
        global passworddictionarypath
        passworddictionarypath = file_path



def button_clicked4():
    # Function for the big button
    global text_box
    text_input_value = text_box.text()
    if len(text_input_value) != 32:
        msg_box1 = QMessageBox()
        msg_box1.setWindowTitle("VADER")
        msg_box1.setText("Your hash is not the correct format.")
        msg_box1.setIcon(QMessageBox.Information)  # Optional: Set icon (e.g., Information)
        msg_box1.setStandardButtons(QMessageBox.Ok)  # Set button (e.g., Ok)
        msg_box1.exec_()  # Display the first popup
        return

    global passworddictionarypath
    if passworddictionarypath == "":
        msg_box1 = QMessageBox()
        msg_box1.setWindowTitle("VADER")
        msg_box1.setText("Please select your dictionary file.")
        msg_box1.setIcon(QMessageBox.Information)  # Optional: Set icon (e.g., Information)
        msg_box1.setStandardButtons(QMessageBox.Ok)  # Set button (e.g., Ok)
        msg_box1.exec_()  # Display the first popup
        return


    #print(text_input_value)
    # Create a QMessageBox for the first popup
    msg_box1 = QMessageBox()
    msg_box1.setWindowTitle("VADER")
    msg_box1.setText("Once you click OK the recovery will begin.")
    msg_box1.setIcon(QMessageBox.Information)  # Optional: Set icon (e.g., Information)
    msg_box1.setStandardButtons(QMessageBox.Ok)  # Set button (e.g., Ok)
    msg_box1.exec_()  # Display the first popup
    #time.sleep(5)
    #show_second_popup()

    dictionary_path = passworddictionarypath
    hash_type = "md5"
    password = text_input_value
    result = subprocess.run(["./VADER", dictionary_path,hash_type, password], capture_output=True, text=True, check=True)
    #print("C++ program successfully executed.")
    output_lines = result.stdout.splitlines()
    #print(output_lines)

    message = "The password was not recovered."
    if len(output_lines) == 5:
        print(output_lines[2].split(":")[1])
        message = "The recovered password was: " + output_lines[2].split(":")[1]
    msg_box1 = QMessageBox()
    msg_box1.setWindowTitle("VADER")
    msg_box1.setText(message)
    msg_box1.setIcon(QMessageBox.Information)  # Optional: Set icon (e.g., Information)
    msg_box1.setStandardButtons(QMessageBox.Ok)  # Set button (e.g., Ok)
    msg_box1.exec_()  # Display the first popup
    # Create a timer object for the 10-second wait


class MyApp(QWidget):
    global self
    def __init__(self):
        super().__init__()
        self.initUI()

    def initUI(self):
        self.setWindowTitle("Versatile Accelerated Digital Encryption Recovery")
        dark = QColor('#323d42')
        light = QColor('#d8d8d9')
        self.setFixedSize(700, 500)
        self.setStyleSheet(f"background-color: {dark.name()};")

        # Layout
        layout = QVBoxLayout(self)
        layout.setContentsMargins(20, 20, 20, 20)
        layout.addSpacing(100)

        # Image
        image_path = "GUI/vader_logo.png"  # Replace with your image path
        pixmap = QPixmap(image_path)
        image_label = QLabel()
        image_label.setPixmap(pixmap)
        layout.addWidget(image_label, alignment=Qt.AlignCenter)

        # Text box
        global text_box
        text_box = QLineEdit()
        text_box.setFont(QFont("Arial", 14))
        text_box.setStyleSheet(f"background-color: {light.name()}; border-radius: 10px; color: {dark.name()}; padding-left: 20px;")
        text_box.setFixedHeight(30)  # Adjust height
        text_box.setFixedWidth(600)

        layout.addWidget(text_box)
        layout.addWidget(text_box, alignment=Qt.AlignCenter)

        layout.addSpacing(-40)

        # Button row
        button_row_layout = QHBoxLayout()
        button_row_layout.addSpacing(10)
        
        # Buttons 1, 2, 3
        for button_text, clicked_function in [("     Hashes     ", button_clicked1), ("   Personal Info   ", button_clicked2), ("   Dictionary   ", button_clicked3)]:
            button = self.create_button(button_text, clicked_function, light, dark)
            button.setFixedSize(160, 40)  # Increase button height
            button_row_layout.addWidget(button)
            # Add spacing between buttons
            button_row_layout.addSpacing(10)

        # for button_text, clicked_function in [("     Hashes     ", lambda self: button_clicked1(self)), ("   Personal Info   ", lambda self: button_clicked2), ("   Dictionary   ", lambda self: button_clicked3)]:
        #     button = self.create_button(button_text, clicked_function, light, dark)
        #     button.setFixedSize(160, 40)  # Increase button height
        #     button_row_layout.addWidget(button)
        #     # Add spacing between buttons
        #     button_row_layout.addSpacing(10)

        spacer = QSpacerItem(40, 20, QSizePolicy.Expanding, QSizePolicy.Minimum)
        layout.addItem(spacer)
        layout.addLayout(button_row_layout)
        spacer = QSpacerItem(40, 20, QSizePolicy.Expanding, QSizePolicy.Minimum)
        layout.addItem(spacer)

        # Big button
        button4 = self.create_button_Start("             Start             ", button_clicked4, light, dark, font_size=12)
        button4.setFixedSize(250, 40)  # Set width and height
        layout.addWidget(button4, alignment=Qt.AlignCenter)
        #layout.addSpacing(50)
        self.setLayout(layout)
        self.show()

    def create_button_Start(self, text, clicked_function, background_color, text_color, font_size=12):
        button = QPushButton(text)
        button.clicked.connect(clicked_function)
        button.setFont(QFont("Arial", font_size))
        button.setStyleSheet(
            f"background-color: {background_color.name()}; color: {text_color.name()}; border-radius: 10px;"
        )
        button.setIcon(QIcon("Arrow.png"))
        button.setLayoutDirection(Qt.RightToLeft)
        return button

    def create_button(self, text, clicked_function, background_color, text_color, font_size=12):
        button = QPushButton(text)
        button.clicked.connect(clicked_function)
        button.setFont(QFont("Arial", font_size))
        button.setStyleSheet(
            f"background-color: {background_color.name()}; color: {text_color.name()}; border-radius: 10px;"
        )
        button.setIcon(QIcon('uploadSymbol.png'))
        
        return button
        


if __name__ == "__main__":
    app = QApplication([])
    window = MyApp()
    sys.exit(app.exec_())
