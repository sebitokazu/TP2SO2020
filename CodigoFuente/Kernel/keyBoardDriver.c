#include "keyBoardDriver.h"

#include <interrupts.h>
#include <naiveConsole.h>

#include "video_driver.h"

#define BUFFER_SIZE 50
#define LEFT_CTRL 0x1D
#define LEFT_SHIFT 0x2A
#define RIGHT_SHIFT 0x36
#define LEFT_CTRL_RELEASE -99
#define LEFT_SHIFT_RELEASE -86
#define RIGHT_SHIFT_RELEASE -74

extern unsigned char getKeyboardScancode();

static void saveInBuffer(char c);

static unsigned char asscode[58][2] = {
    {0, 0}, {0, 0}, {'1', '!'}, {'2', '@'}, {'3', '#'}, {'4', '$'}, {'5', '%'}, {'6', '^'}, {'7', '&'}, {'8', '*'}, {'9', '('}, {'0', ')'}, {'-', '_'}, {'=', '+'}, {'\b', '\b'}, {'\t', '\t'}, {'q', 'Q'}, {'w', 'W'}, {'e', 'E'}, {'r', 'R'}, {'t', 'T'}, {'y', 'Y'}, {'u', 'U'}, {'i', 'I'}, {'o', 'O'}, {'p', 'P'}, {'[', '}'}, {']', '}'}, {'\n', '\n'}, {0, 0}, {'a', 'A'}, {'s', 'S'},  //caps lock
    {'d', 'D'},
    {'f', 'F'},
    {'g', 'G'},
    {'h', 'H'},
    {'j', 'J'},
    {'k', 'K'},
    {'l', 'L'},
    {';', ':'},
    {'\'', '\"'},
    {'`', '~'},
    {1, 0},
    {'\\', '|'},  //ctrl
    {'z', 'Z'},
    {'x', 'X'},
    {'c', 'C'},
    {'v', 'V'},
    {'b', 'B'},
    {'n', 'N'},
    {'m', 'M'},
    {',', '<'},
    {'.', '>'},
    {'/', '?'},
    {0, 0},
    {1, 0},  //shift, ctrl
    {0, 0},
    {' ', ' '}  //alt, space
};

static char buffer[BUFFER_SIZE];
static unsigned int bufferSize = 0;
static unsigned int currentInsertionIndex = 0;
static unsigned int readingIndex = 0;
static unsigned int ctrl = 0, shift = 0;

void keyboard_handler() {
    char scanCode = getKeyboardScancode();
    switch (scanCode) {
        case LEFT_CTRL:
            ctrl = 1;
            saveInBuffer(1);
            break;
        case LEFT_SHIFT:
        case RIGHT_SHIFT:
            shift = 1;
            break;
        case LEFT_CTRL_RELEASE:
            ctrl = 0;
            saveInBuffer(-1);
            break;
        case LEFT_SHIFT_RELEASE:
        case RIGHT_SHIFT_RELEASE:
            shift = 0;
            break;
        default:
            if (scanCode < 129) {
                unsigned char keyPress = asscode[scanCode][shift];
                saveInBuffer(keyPress);
            }
    }
}

static void saveInBuffer(char c) {
    if (ctrl && c == 'c') {
        uint64_t fgpid = getForegroundPID();
        if (fgpid != 1) {
            blockProcess(1);
            removeProcess(fgpid);
            setForegroundPID(1);
            return;
        }
    }
    buffer[currentInsertionIndex] = c;
    if (bufferSize < BUFFER_SIZE)
        bufferSize++;
    else {
        if (currentInsertionIndex == readingIndex)
            readingIndex = (readingIndex + 1) % BUFFER_SIZE;
    }
    currentInsertionIndex = (currentInsertionIndex + 1) % BUFFER_SIZE;
}

int emptyBuffer() {
    return bufferSize == 0;
}

int getBufferSize() {
    return bufferSize;
}

void readChar(char *dest) {
    if (bufferSize > 0) {
        *dest = buffer[readingIndex];
        bufferSize--;
    } else
        *dest = 0;  //null
    readingIndex = (readingIndex + 1) % BUFFER_SIZE;
}

void readChars(char *dest, int size) {
    int i = 0;
    while (size--) {
        dest[i++] = buffer[readingIndex];
        readingIndex = (readingIndex + 1) % BUFFER_SIZE;
    }
    dest[i] = 0;
}
