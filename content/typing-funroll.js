const texts = [
  "-funroll",
  "-loops",
  "-fomit-frame-pointer",
  "-fnever-follow-instructions",
];
const typingSpeed = 50; // 50 ms per char
const backspacingSpeed = 30; // ms per character
const pauseBetweenTexts = 800; // ms pause before starting to type the next text
const longPause = 3000; // ms long pause after completing the loop

function typeText(textIndex) {
  const output = document.getElementById("-funroll");
  const text = texts[textIndex];
  let index = 0;
  console.log(`line 16 ${text} ${index}`);

  const typingInterval = setInterval(() => {
    if (index < text.length) {
      output.textContent += text.charAt(index);
      index++;
    } else {
      clearInterval(typingInterval);
      console.log("line 24 " + text);
      switch (textIndex) {
        case 0:
          console.log("case 0 " + text);
          setTimeout(() => typeText(textIndex + 1), longPause);
          break;
        default:
          console.log("case default " + text);
          setTimeout(() => backspaceText(textIndex), pauseBetweenTexts);
          break;
      }
    }
  }, typingSpeed);
}

function backspaceText(textIndex) {
  const output = document.getElementById("-funroll");
  let index = output.textContent.length;

  const backspacingInterval = setInterval(() => {
    if (index > 0) {
      output.textContent = output.textContent.slice(0, -1);
      index--;
    } else {
      clearInterval(backspacingInterval);
      setTimeout(() => {
        typeText((textIndex + 1) % texts.length);
      }, pauseBetweenTexts);
    }
  }, backspacingSpeed);
}

// Start the typing effect
setTimeout(() => typeText(1), longPause);
