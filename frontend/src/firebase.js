
import { initializeApp } from "firebase/app";
import { getAnalytics } from "firebase/analytics";
// TODO: Add SDKs for Firebase products that you want to use
// https://firebase.google.com/docs/web/setup#available-libraries

// Your web app's Firebase configuration
// For Firebase JS SDK v7.20.0 and later, measurementId is optional
const firebaseConfig = {
    apiKey: "AIzaSyCL5wMAF6hwpD6evUgkUYGs5oDVvmvVUso",
    authDomain: "cellar-automation-system.firebaseapp.com",
    projectId: "cellar-automation-system",
    storageBucket: "cellar-automation-system.firebasestorage.app",
    messagingSenderId: "153812867435",
    appId: "1:153812867435:web:e8fe770941751fe320d42e",
    measurementId: "G-DGGMTM0LS0"
};

// Initialize Firebase
const app = initializeApp(firebaseConfig);
const analytics = getAnalytics(app);