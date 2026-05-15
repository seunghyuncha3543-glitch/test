import * as THREE from 'https://unpkg.com/three@0.161.0/build/three.module.js';

const canvas = document.getElementById('app');
const renderer = new THREE.WebGLRenderer({ canvas, antialias: true });
renderer.setSize(innerWidth, innerHeight);
renderer.setPixelRatio(devicePixelRatio);

const scene = new THREE.Scene();
scene.background = new THREE.Color(0x9eb7cf);
scene.fog = new THREE.Fog(0x9eb7cf, 180, 1200);

const camera = new THREE.PerspectiveCamera(65, innerWidth / innerHeight, 0.1, 2500);

const sun = new THREE.DirectionalLight(0xffffff, 1.3);
sun.position.set(200, 250, 100);
scene.add(sun, new THREE.AmbientLight(0xffffff, 0.45));

const road = new THREE.Mesh(new THREE.PlaneGeometry(24, 4000), new THREE.MeshStandardMaterial({ color: 0x303030 }));
road.rotation.x = -Math.PI / 2;
scene.add(road);

const desert = new THREE.Mesh(new THREE.PlaneGeometry(1200, 4000), new THREE.MeshStandardMaterial({ color: 0xa58f6f }));
desert.position.y = -0.05;
desert.rotation.x = -Math.PI / 2;
scene.add(desert);

for (let i = 0; i < 800; i++) {
  const c = new THREE.Mesh(new THREE.CylinderGeometry(0.1, 0.1, 1.6), new THREE.MeshStandardMaterial({ color: 0x3d6d38 }));
  c.position.set((Math.random() - 0.5) * 1000, 0.8, (Math.random() - 0.5) * 3500);
  if (Math.abs(c.position.x) < 16) continue;
  scene.add(c);
}

const car = new THREE.Group();
const body = new THREE.Mesh(new THREE.BoxGeometry(2.05, 0.62, 4.35), new THREE.MeshStandardMaterial({ color: 0x5e656f, metalness: 0.7, roughness: 0.35 }));
body.position.y = 0.82;
car.add(body);
scene.add(car);

const state = {
  mass: 1650, dragCoefficient: 0.30, rollingResistance: 13.2, maxBrake: 16500,
  maxSteer: 0.52, speed: 0, yaw: 0, rpm: 900, gear: 1, sport: false,
  absActive: false, tcsActive: false, camMode: 0
};

const gears = [3.6, 2.19, 1.59, 1.24, 1.0, 0.82, 0.68, 0.58];
const reverseRatio = 3.25;
let shiftLock = 0;
const keys = new Set();

addEventListener('keydown', (e) => {
  if (e.repeat) return;
  keys.add(e.code);
  if (e.code === 'KeyC') state.camMode = (state.camMode + 1) % 3;
  if (e.code === 'KeyR') resetCar();
});
addEventListener('keyup', (e) => keys.delete(e.code));

function torqueCurve(rpm) {
  if (rpm < 1000) return 360;
  if (rpm < 3500) return 420 + (rpm - 1000) * (190 / 2500);
  if (rpm < 6500) return 610 - (rpm - 3500) * (80 / 3000);
  return 530 - (rpm - 6500) * 0.11;
}

function resetCar() {
  car.position.set(0, 0, 0);
  state.speed = 0; state.yaw = 0; state.rpm = 900; state.gear = 1;
}

function update(dt) {
  const throttle = keys.has('KeyW') ? 1 : 0;
  const brake = keys.has('KeyS') ? 1 : 0;
  const steer = (keys.has('KeyA') ? 1 : 0) - (keys.has('KeyD') ? 1 : 0);
  const handbrake = keys.has('Space');
  state.sport = keys.has('ShiftLeft') || keys.has('ShiftRight');

  const ratio = (brake > 0.2 && Math.abs(state.speed) < 1.2) ? -reverseRatio : gears[state.gear - 1];
  state.rpm = THREE.MathUtils.clamp(Math.abs(state.speed) * 115 * Math.abs(ratio) + 900, 900, 7800);

  if (shiftLock <= 0) {
    const upRpm = state.sport ? 7250 : 6800;
    if (state.rpm > upRpm && state.gear < gears.length && throttle > 0.2) { state.gear++; shiftLock = 0.16; }
    if (state.rpm < 2200 && state.gear > 1) { state.gear--; shiftLock = 0.16; }
  } else shiftLock -= dt;

  let drive = shiftLock > 0 ? 0 : throttle * torqueCurve(state.rpm) * ratio * 0.88;
  state.tcsActive = false;
  if (throttle > 0.3 && Math.abs(steer) > 0.45 && state.speed < 20) { drive *= 0.58; state.tcsActive = true; }

  let brakeForce = brake * state.maxBrake;
  if (handbrake) brakeForce += 6000;
  state.absActive = false;
  if (brake > 0.4 && state.speed > 30 && Math.abs(steer) > 0.28) { brakeForce *= 0.65; state.absActive = true; }

  const drag = state.dragCoefficient * state.speed * state.speed * Math.sign(state.speed);
  const roll = state.rollingResistance * state.speed;
  const accel = (drive - brakeForce * Math.sign(state.speed || 1) - drag - roll) / state.mass;
  state.speed += accel * dt * 45;
  state.speed = THREE.MathUtils.clamp(state.speed, -50, 86);

  const steerScale = THREE.MathUtils.clamp(1 - Math.abs(state.speed) / 120, 0.3, 1);
  state.yaw += steer * state.maxSteer * steerScale * dt * (state.speed / 30);

  car.rotation.y = state.yaw;
  const forward = new THREE.Vector3(0, 0, 1).applyAxisAngle(new THREE.Vector3(0, 1, 0), state.yaw);
  car.position.addScaledVector(forward, state.speed * dt);

  const speedKmh = Math.abs(state.speed) * 3.6;
  const fov = THREE.MathUtils.lerp(62, 84, Math.min(speedKmh / 310, 1));
  camera.fov = fov;
  camera.updateProjectionMatrix();

  if (state.camMode === 0) {
    const camOffset = new THREE.Vector3(0, 2.6, -7.8).applyAxisAngle(new THREE.Vector3(0, 1, 0), state.yaw);
    camera.position.lerp(car.position.clone().add(camOffset), 0.12);
  } else if (state.camMode === 1) {
    const camOffset = new THREE.Vector3(0, 1.7, -4.5).applyAxisAngle(new THREE.Vector3(0, 1, 0), state.yaw);
    camera.position.lerp(car.position.clone().add(camOffset), 0.18);
  } else {
    const camOffset = new THREE.Vector3(0, 1.2, 0.8).applyAxisAngle(new THREE.Vector3(0, 1, 0), state.yaw);
    camera.position.lerp(car.position.clone().add(camOffset), 0.18);
  }
  camera.lookAt(car.position.x, 1.1, car.position.z + 8 * Math.cos(state.yaw));

  document.getElementById('speed').textContent = speedKmh.toFixed(0);
  document.getElementById('gear').textContent = state.speed < -1 ? 'R' : state.gear;
  document.getElementById('rpm').textContent = state.rpm.toFixed(0);
  document.getElementById('abs').textContent = state.absActive ? 'ABS ON' : 'ABS';
  document.getElementById('tcs').textContent = state.tcsActive ? 'TCS ON' : 'TCS';
}

let prev = performance.now();
function loop(t) {
  const dt = Math.min((t - prev) / 1000, 0.033);
  prev = t;
  update(dt);
  renderer.render(scene, camera);
  requestAnimationFrame(loop);
}
requestAnimationFrame(loop);

addEventListener('resize', () => {
  camera.aspect = innerWidth / innerHeight;
  camera.updateProjectionMatrix();
  renderer.setSize(innerWidth, innerHeight);
});
