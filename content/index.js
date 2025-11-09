// 1. typing effect
(() => {
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
    //console.log(`line 16 ${text} ${index}`);

    const typingInterval = setInterval(() => {
      //console.log(output.scrollLeft);
      output.scrollLeft = output.scrollWidth;
      //console.log(output.scrollLeft);
      if (index < text.length) {
        output.textContent += text.charAt(index);
        index++;
      } else {
        clearInterval(typingInterval);
        //console.log("line 24 " + text);
        switch (textIndex) {
          case 0:
            //console.log("case 0 " + text);
            setTimeout(() => typeText(textIndex + 1), longPause);
            break;
          default:
            //console.log("case default " + text);
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
})();

// 2. physics fling effect
const physics = (() => {
  document.body.scrollTop = 0; // safari
  document.documentElement.scrollTop = 0; // firefox etc.

  let freezeTriggered = false;
  let allIntervals = [];

  function rememberInterval(id) {
    allIntervals.push(id);
  }

  function loadMatter(cb) {
    if (window.Matter) return cb();
    const s = document.createElement("script");
    // NOTE: relies on matter.js for silly effect
    s.src = "https://cdnjs.cloudflare.com/ajax/libs/matter-js/0.19.0/matter.min.js";
    s.onload = cb;
    document.head.appendChild(s);
  }

  loadMatter(() => {
    // https://github.com/liabru/matter-js/wiki/Getting-started
    const { Engine, Runner, Bodies, Body, Composite, Events } = Matter;

    const engine = Engine.create();
    engine.gravity.y = 1.3; // https://github.com/liabru/matter-js/blob/acb99b6f8784c809b940f1d2cf745427e088e088/examples/gravity.js#L44
    engine.enableSleeping = false;

    const runner = Runner.create();
    Runner.run(runner, engine);

    const bodies = [];

    // visible els above minimum size
    const elements = [...document.querySelectorAll("body *")].filter(el => {
      const computedStyle = getComputedStyle(el);
      if (computedStyle.visibility === "hidden" || computedStyle.display === "none" ||
        el.closest("script,style,meta,head,link")) return false;
      const r = el.getBoundingClientRect();
      return r.width > 12 && r.height > 12;
    });

    // collision group to ignore each other
    const collisionGroup = Matter.Body.nextGroup(true);

    elements.forEach(el => {
      const r = el.getBoundingClientRect();

      document.body.appendChild(el);
      Object.assign(el.style, {
        position: "fixed",
        left: `${r.left / 2}px`,
        top: `${r.top}px`,
        width: `${r.width}px`,
        height: `${r.height}px`,
        margin: "0",
        pointerEvents: "none",
        transformOrigin: "50% 50%",
        zIndex: 999999
      });

      const body = Bodies.rectangle(
        r.left + r.width,
        r.top + r.height,
        r.width, r.height,
        {
          collisionFilter: { group: collisionGroup }
        }
      );

      body.el = el;
      bodies.push(body);
      Composite.add(engine.world, body);
    });

    // https://github.com/liabru/matter-js/blob/master/examples/restitution.js
    const floor = Bodies.rectangle(innerWidth, innerHeight + 50, innerWidth, 100, { isStatic: true, restitution: 1 });
    const ceiling = Bodies.rectangle(innerWidth, -50, innerWidth, 100, { isStatic: true });
    const leftWall = Bodies.rectangle(-50, innerHeight, 100, innerHeight, { isStatic: true });
    const rightWall = Bodies.rectangle(innerWidth + 50, innerHeight, 100, innerHeight, { isStatic: true });

    Composite.add(engine.world, [floor, ceiling, leftWall, rightWall]);

    Events.on(engine, "collisionStart", (ev) => {
      for (const pair of ev.pairs) {
        const b = pair.bodyA.el ? pair.bodyA : pair.bodyB.el ? pair.bodyB : null;
        if (!b) continue;

        if (pair.bodyA === floor || pair.bodyB === floor) {
          // results have been inconsistent so instead of trying to
          // stop entropy/glitches we randomly increment omega, v

          const torqueIncrement = (Math.random() - 0.5) * 2;
          Body.setAngularVelocity(b, b.angularVelocity + torqueIncrement);

          // set max for angular velocity
          const maxSpin = 1.4;
          if (Math.abs(b.angularVelocity) > maxSpin) {
            Body.setAngularVelocity(b, Math.sign(b.angularVelocity) * maxSpin);
          }

          // small velocity kick also
          Body.setVelocity(b, {
            x: b.velocity.x + (Math.random() - 0.5) * 20,
            y: b.velocity.y
          });
          Body.setAngularVelocity(b, (Math.random() - 0.5) * 20
          );

        }
      }
    });

    rememberInterval(
      setInterval(() => {
        for (const b of bodies) {
          Body.applyForce(b, b.position, {
            x: (Math.random() - 0.5) * 0.01,
            y: 0
          });
        }
      }, 35)
    );
    rememberInterval(
      setInterval(() => {
        for (const b of bodies) {
          if (Math.abs(b.velocity.y) < 0.3 && Math.abs(b.velocity.x) < 0.3 && b.position.y > innerHeight * 0.7) {
          }
        }
      }, 100)
    );
    // also small random kick for slow bodies
    rememberInterval(setInterval(() => {
      for (const b of bodies) {
        const speed = Math.hypot(b.velocity.x, b.velocity.y);
        if (speed < 1.2) {
          Body.applyForce(b, b.position, {
            x: (Math.random() - 0.5) * 0.01,
            y: (Math.random() - 0.5) * -0.01
          });
        }
      }
    }, 80));

    function freezeFunc() {
      // guard, probably not needed
      if (freezeTriggered) return;
      freezeTriggered = true;

      Runner.stop(runner);

      for (const id of allIntervals) clearInterval(id);

      const rightX = window.innerWidth * 0.75;
      const topY = window.innerHeight * 0.15;
      const spacing = window.innerHeight * 0.05;

      bodies.forEach((b, i) => {
        const x = rightX;
        const y = topY + i * spacing;

        Body.setPosition(b, { x, y });
        Body.setVelocity(b, { x: 0, y: 0 });
        Body.setAngularVelocity(b, 0);
        //Body.setAngle(b, 0) // unnatural

        b.el.style.transform =
          `translate(${x - b.el.offsetWidth / 2}px, ${y - b.el.offsetHeight / 2}px) rotate(0rad)`;
      });

      let text = document.getElementById("recompile-it");
      let link = document.getElementById("configs");

      text.style.pointerEvents = "auto";
      text.style.zIndex = "9999999";
      text.style.textAlign = "left";
      text.style.color = "#fc4646";

      link.style.pointerEvents = "auto";
      link.style.zIndex = "99999999";
      link.style.color = "#000";
      link.style.width = "12ch";
      link.className = "bright"

      const yBase = topY + 20;
      text.style.left = `${rightX * 0.8}px`;
      text.style.top = `${yBase / 2}px`;
      link.style.left = `${rightX * 0.8}px`;
      link.style.top = `${yBase / 2}px`;
    }

    (function renderLoop() {
      // OOB guards
      const centerX = innerWidth / 2;
      const centerY = innerHeight / 2;
      const boundsLimit = innerWidth / 2;

      for (const b of bodies) {
        b.el.style.transform =
          `translate(${b.position.x - b.el.offsetWidth / 2}px, ${b.position.y - b.el.offsetHeight / 2}px) rotate(${b.angle}rad)`;

        // tp randomly in center
        if (Math.abs(b.position.x) > boundsLimit || Math.abs(b.position.y) > boundsLimit) {
          Body.setPosition(b, { x: centerX * Math.random(), y: centerY * Math.random() });
          Body.setVelocity(b, { x: Math.random() * 4, y: Math.random() * 4 });
        }
      }
      requestAnimationFrame(renderLoop);
    })();

    setTimeout(
      () => window.addEventListener("pointerdown", freezeFunc, { once: true }),
      50);
  });
});
