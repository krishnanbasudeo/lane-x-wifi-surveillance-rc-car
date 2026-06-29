document.addEventListener("DOMContentLoaded", function () {
  /* Reveal animation */
  const revealElements = document.querySelectorAll(".reveal");

  const revealObserver = new IntersectionObserver(
    function (entries) {
      entries.forEach(function (entry) {
        if (entry.isIntersecting) {
          entry.target.classList.add("active");
        }
      });
    },
    {
      threshold: 0.12
    }
  );

  revealElements.forEach(function (element) {
    revealObserver.observe(element);
  });

  /* Cinematic star + meteor background */
  const canvas = document.getElementById("robotics-bg");
  const ctx = canvas.getContext("2d");

  let width = 0;
  let height = 0;

  let farStars = [];
  let midStars = [];
  let nearStars = [];
  let brightStars = [];
  let shootingStars = [];
  let nebulaPatches = [];

  let mouseX = 0;
  let mouseY = 0;
  let scrollYValue = 0;
  let showerCooldown = 0;

  function random(min, max) {
    return Math.random() * (max - min) + min;
  }

  function resizeCanvas() {
    width = canvas.width = window.innerWidth;
    height = canvas.height = window.innerHeight;

    createNebula();
    createStars();
  }

  function createNebula() {
    nebulaPatches = [];

    for (let i = 0; i < 6; i++) {
      nebulaPatches.push({
        x: random(0, width),
        y: random(0, height),
        radius: random(180, 360),
        alpha: random(0.04, 0.09),
        color: Math.random() > 0.5 ? "56, 189, 248" : "37, 99, 235"
      });
    }
  }

  function generateStars(count, radiusMin, radiusMax, alphaMin, alphaMax, drift) {
    const arr = [];

    for (let i = 0; i < count; i++) {
      arr.push({
        x: Math.random() * width,
        y: Math.random() * height,
        radius: random(radiusMin, radiusMax),
        alpha: random(alphaMin, alphaMax),
        twinkleSpeed: random(0.004, 0.02),
        twinkleDirection: Math.random() > 0.5 ? 1 : -1,
        driftX: random(-drift, drift),
        driftY: random(-drift, drift),
        pulse: random(0, Math.PI * 2)
      });
    }

    return arr;
  }

  function createStars() {
    const area = width * height;

    farStars = generateStars(
      Math.floor(area / 17000),
      0.5,
      1.3,
      0.18,
      0.55,
      0.03
    );

    midStars = generateStars(
      Math.floor(area / 24000),
      0.9,
      2.0,
      0.28,
      0.78,
      0.05
    );

    nearStars = generateStars(
      Math.floor(area / 36000),
      1.4,
      2.8,
      0.42,
      0.95,
      0.08
    );

    brightStars = generateStars(
      Math.floor(area / 85000),
      2.2,
      4.2,
      0.65,
      1,
      0.02
    );
  }

  function createShootingStar(extraBright = false) {
    const fromLeft = Math.random() > 0.5;
    const startX = fromLeft
      ? random(-160, width * 0.35)
      : random(width * 0.65, width + 160);

    const startY = random(0, height * 0.55);
    const direction = fromLeft ? 1 : -1;

    shootingStars.push({
      x: startX,
      y: startY,
      vx: random(8, 13) * direction,
      vy: random(4.5, 8),
      length: extraBright ? random(260, 420) : random(170, 300),
      size: extraBright ? random(2.8, 4.8) : random(1.8, 3.2),
      opacity: 1,
      fade: extraBright ? random(0.006, 0.011) : random(0.009, 0.016),
      extraBright: extraBright
    });
  }

  function createMeteorShower() {
    const count = Math.floor(random(3, 6));

    for (let i = 0; i < count; i++) {
      setTimeout(function () {
        createShootingStar(true);
      }, i * random(150, 280));
    }
  }

  function updateStar(star) {
    star.alpha += star.twinkleSpeed * star.twinkleDirection;

    if (star.alpha >= 1) {
      star.alpha = 1;
      star.twinkleDirection = -1;
    }

    if (star.alpha <= 0.12) {
      star.alpha = 0.12;
      star.twinkleDirection = 1;
    }

    star.x += star.driftX;
    star.y += star.driftY;
    star.pulse += 0.02;

    if (star.x < 0) star.x = width;
    if (star.x > width) star.x = 0;
    if (star.y < 0) star.y = height;
    if (star.y > height) star.y = 0;
  }

  function drawStarLayer(stars, glowMultiplier, parallaxStrength, scrollStrength) {
    stars.forEach(function (star) {
      updateStar(star);

      const offsetX = (mouseX - width / 2) * parallaxStrength;
      const offsetY =
        (mouseY - height / 2) * parallaxStrength +
        (scrollYValue * scrollStrength);

      const drawX = star.x + offsetX;
      const drawY = star.y + offsetY;
      const pulseScale = 1 + Math.sin(star.pulse) * 0.08;

      ctx.beginPath();
      ctx.arc(drawX, drawY, star.radius * pulseScale, 0, Math.PI * 2);
      ctx.fillStyle = "rgba(190, 230, 255, " + star.alpha + ")";
      ctx.shadowBlur = 7 * glowMultiplier;
      ctx.shadowColor = "rgba(125, 211, 252, 0.95)";
      ctx.fill();
      ctx.shadowBlur = 0;
    });
  }

  function drawBrightStars() {
    brightStars.forEach(function (star) {
      updateStar(star);

      const offsetX = (mouseX - width / 2) * 0.012;
      const offsetY = (mouseY - height / 2) * 0.012 + (scrollYValue * 0.015);
      const drawX = star.x + offsetX;
      const drawY = star.y + offsetY;
      const pulseScale = 1 + Math.sin(star.pulse * 1.4) * 0.18;

      /* star glow */
      ctx.beginPath();
      ctx.arc(drawX, drawY, star.radius * pulseScale, 0, Math.PI * 5);
      ctx.fillStyle = "rgba(220, 245, 255, " + star.alpha + ")";
      ctx.shadowBlur = 18;
      ctx.shadowColor = "rgba(255, 255, 255, 0.95)";
      ctx.fill();
      ctx.shadowBlur = 0;

      /* little sparkle cross */
      ctx.strokeStyle = "rgba(220, 245, 255, " + (star.alpha * 0.7) + ")";
      ctx.lineWidth = 1;
      ctx.beginPath();
      ctx.moveTo(drawX - star.radius * 2.2, drawY);
      ctx.lineTo(drawX + star.radius * 2.2, drawY);
      ctx.moveTo(drawX, drawY - star.radius * 2.2);
      ctx.lineTo(drawX, drawY + star.radius * 2.2);
      ctx.stroke();
    });
  }

  function drawNebula() {
    nebulaPatches.forEach(function (patch) {
      const gradient = ctx.createRadialGradient(
        patch.x,
        patch.y,
        0,
        patch.x,
        patch.y,
        patch.radius
      );

      gradient.addColorStop(0, "rgba(" + patch.color + ", " + patch.alpha + ")");
      gradient.addColorStop(0.4, "rgba(" + patch.color + ", " + (patch.alpha * 0.45) + ")");
      gradient.addColorStop(1, "rgba(" + patch.color + ", 0)");

      ctx.beginPath();
      ctx.fillStyle = gradient;
      ctx.arc(patch.x, patch.y, patch.radius, 0, Math.PI * 2);
      ctx.fill();
    });
  }

  function drawAmbientGlow() {
    const glow1 = ctx.createRadialGradient(
      width * 0.18, height * 0.18, 0,
      width * 0.18, height * 0.18, 280
    );
    glow1.addColorStop(0, "rgba(56, 189, 248, 0.12)");
    glow1.addColorStop(1, "rgba(56, 189, 248, 0)");

    ctx.fillStyle = glow1;
    ctx.fillRect(0, 0, width, height);

    const glow2 = ctx.createRadialGradient(
      width * 0.82, height * 0.22, 0,
      width * 0.82, height * 0.22, 260
    );
    glow2.addColorStop(0, "rgba(37, 99, 235, 0.11)");
    glow2.addColorStop(1, "rgba(37, 99, 235, 0)");

    ctx.fillStyle = glow2;
    ctx.fillRect(0, 0, width, height);
  }

  function drawShootingStars() {
    for (let i = shootingStars.length - 1; i >= 0; i--) {
      const star = shootingStars[i];

      const tailX = star.x - star.vx * star.length * 0.09;
      const tailY = star.y - star.vy * star.length * 0.09;

      const gradient = ctx.createLinearGradient(star.x, star.y, tailX, tailY);
      gradient.addColorStop(0, "rgba(255, 255, 255, " + star.opacity + ")");
      gradient.addColorStop(0.12, "rgba(180, 235, 255, " + star.opacity + ")");
      gradient.addColorStop(0.45, "rgba(96, 165, 250, " + (star.opacity * 0.7) + ")");
      gradient.addColorStop(1, "rgba(96, 165, 250, 0)");

      ctx.beginPath();
      ctx.moveTo(star.x, star.y);
      ctx.lineTo(tailX, tailY);
      ctx.strokeStyle = gradient;
      ctx.lineWidth = star.size;
      ctx.lineCap = "round";
      ctx.shadowBlur = star.extraBright ? 30 : 22;
      ctx.shadowColor = star.extraBright
        ? "rgba(255, 255, 255, 1)"
        : "rgba(125, 211, 252, 1)";
      ctx.stroke();
      ctx.shadowBlur = 0;

      /* bright meteor head */
      ctx.beginPath();
      ctx.arc(star.x, star.y, star.size * (star.extraBright ? 1.2 : 0.9), 0, Math.PI * 2);
      ctx.fillStyle = "rgba(255, 255, 255, " + star.opacity + ")";
      ctx.shadowBlur = star.extraBright ? 24 : 16;
      ctx.shadowColor = "rgba(255, 255, 255, 1)";
      ctx.fill();
      ctx.shadowBlur = 0;

      star.x += star.vx;
      star.y += star.vy;
      star.opacity -= star.fade;

      if (
        star.opacity <= 0 ||
        star.x < -500 ||
        star.x > width + 500 ||
        star.y > height + 350
      ) {
        shootingStars.splice(i, 1);
      }
    }
  }

  function animate() {
    ctx.clearRect(0, 0, width, height);

    drawAmbientGlow();
    drawNebula();

    drawStarLayer(farStars, 0.7, 0.004, 0.006);
    drawStarLayer(midStars, 1.1, 0.008, 0.010);
    drawStarLayer(nearStars, 1.5, 0.013, 0.016);
    drawBrightStars();
    drawShootingStars();

    /* normal meteors */
    if (Math.random() < 0.038 && shootingStars.length < 5) {
      createShootingStar(false);
    }

    /* extra bright meteors */
    if (Math.random() < 0.014 && shootingStars.length < 6) {
      createShootingStar(true);
    }

    /* occasional mini meteor shower */
    if (showerCooldown <= 0 && Math.random() < 0.0025) {
      createMeteorShower();
      showerCooldown = 420;
    }

    if (showerCooldown > 0) {
      showerCooldown--;
    }

    requestAnimationFrame(animate);
  }

  resizeCanvas();
  animate();

  window.addEventListener("resize", resizeCanvas);

  window.addEventListener("mousemove", function (e) {
    mouseX = e.clientX;
    mouseY = e.clientY;
  });

  window.addEventListener("scroll", function () {
    scrollYValue = window.scrollY;
  });
});
