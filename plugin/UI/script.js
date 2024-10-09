import * as Juce from './public/js/juce/javascript/index.js';
import * as THREE from 'three'
import { OrbitControls } from 'three/examples/jsm/controls/OrbitControls.js'
import GUI from 'lil-gui'

//CANVAS
const canvas = document.querySelector('canvas.webgl')

//JUCE
window.__JUCE__.backend.addEventListener(
    "exampleEvent",
    (objectFromCppBackend) => {
        console.log(objectFromCppBackend);
    }
)

const data = window.__JUCE__.initialisationData;

const nativeFunction = Juce.getNativeFunction("nativeFunction");


// Debug
const gui = new GUI()

//SCENE
const scene = new THREE.Scene()

// Set the background color to the gentle complementary color
const taliesenComplement = new THREE.Color(12 / 65, 65 / 202, 5 / 32);  // A soft, warm beige

scene.background = taliesenComplement;

//LIGHT
const directionalLight = new THREE.DirectionalLight(0x00fffc, 1.0)
directionalLight.position.set(0, 1, 0)
scene.add(directionalLight)

directionalLight.intensity = 0.419
gui.add(directionalLight, 'intensity').min(0).max(3).step(0.001).name(' MIDI Velocity')

const ambientLight = new THREE.AmbientLight(0xffffff, 0.1);
scene.add(ambientLight);

ambientLight.intensity = 0.12

gui.add(ambientLight, 'intensity').min(0).max(2).step(0.001).name('Envelope Time')

//________________________________________
//USE MOUSE - HELD DOWN FOR AN ENVELOPE THAT INTRODUCES WHITE NOISE


//COLORS

const taliesenGreen = new THREE.Color(77 / 255, 119 / 255, 95 / 255);

const taliesenRed = new THREE.Color(204 / 255, 85 / 255, 0 / 255);

//WORLD BOUNDS
const worldBounds = {
    xMin: -5,
    xMax: 5,
    yMin: -5,
    yMax: 5,
    zMin: 170,  // Set this closer to the camera for your scroll-based control
    zMax: 800  // Adjust based on how far you want the camera to move back
};



//MATERIAL
const material = new THREE.MeshStandardMaterial({ color: taliesenGreen })
material.roughness = 0.3

//OBJECTS

//SPHERE
const geometry = new THREE.SphereGeometry( 150, 64, 32 )
const mesh = new THREE.Mesh(geometry, material)
mesh.position.x = 0.9
mesh.position.y = -1
mesh.rotation.z = .3
material.wireframe=false;
scene.add(mesh)

//PLANE
const plane = new THREE.PlaneGeometry(10, 10)
const planeMesh = new THREE.Mesh(plane, material)
planeMesh.rotation.x = -Math.PI * 0.5; 
planeMesh.position.set(1, 1, 1)
scene.add(planeMesh)

//HOVER OVER OBJECT FOR LOW_PASS AND HIGH_PASS

//Sizes
const sizes = {
    width: window.innerWidth,
    height: window.innerHeight
}

//AXES HELPER
const axesHelper = new THREE.AxesHelper()
scene.add(axesHelper);

//CAMERA
const camera = new THREE.PerspectiveCamera(75, sizes.width / sizes.height)
camera.position.z = 350
scene.add(camera)

const zControl = gui.add(camera.position, 'z').min(170).max(800).step(0.01).name('Volume')


//CONTROLS
const controls = new OrbitControls(camera, canvas)
controls.enableDamping = true

//JUCE CONTROLS

let clickCount = 0;

//UPDATE GAIN
function calculateGain(zPosition) {
    const zMin = 1;
    const zMax = 1000;
    const gainMin = -24.0;
    const gainMax = 24.0;

    // Map zPosition to the frequency range
    const gain = gainMin + ((zPosition - zMin) / (zMax - zMin)) * (gainMax - freqMin);
    return frequency;
}

canvas.addEventListener("wheel", (event) => {
    let frequency;

    // Adjust frequency based on camera position (or other logic)
    if (event.deltaY > 0 && camera.position.z < worldBounds.zMax) {
        camera.position.z += 0.1;
        frequency = calculateGain(camera.position.z);  // Calculate frequency based on camera position
    } else if (event.deltaY < 0 && camera.position.z > worldBounds.zMin) {
        camera.position.z -= 0.1;
        frequency = calculateGain(camera.position.z);  // Calculate frequency based on camera position
    }

    zControl.updateDisplay()

    if (gain) {
        // Send the frequency value to the JUCE backend
        nativeFunction(gain);  // This calls the native C++ function that updates the slider
    }
});

// Update Sine Wave
//1 Listen for scroll events
//2 Adjust camera position
//3 Map z position to frequency range
//4 update juce

//RENDERER
const renderer = new THREE.WebGLRenderer({
    canvas: canvas
})

renderer.setSize(sizes.width, sizes.height)

renderer.render(scene, camera)

//ANIMATE
const animate = () => {
    requestAnimationFrame(animate);

    // Restrict camera position within the world bounds
    camera.position.x = Math.max(worldBounds.xMin, Math.min(worldBounds.xMax, camera.position.x));
    camera.position.y = Math.max(worldBounds.yMin, Math.min(worldBounds.yMax, camera.position.y));
    camera.position.z = Math.max(worldBounds.zMin, Math.min(worldBounds.zMax, camera.position.z));

    renderer.render(scene, camera);
};
animate();

//RESIZE
window.addEventListener('resize', () => {
    // Update sizes
    sizes.width = window.innerWidth;
    sizes.height = window.innerHeight;

    // Update camera
    camera.aspect = sizes.width / sizes.height;
    camera.updateProjectionMatrix();

    // Update renderer
    renderer.setSize(sizes.width, sizes.height);
});