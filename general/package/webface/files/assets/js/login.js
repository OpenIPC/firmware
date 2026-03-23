/**
 * login.js — Login form handler
 * Copyright (c) OpenIPC  https://openipc.org  MIT License
 */
(function() {
  'use strict';

  // ── DOM references ──────────────────────────────────────────────────────
  var form      = document.getElementById('loginForm');
  var userInput = document.getElementById('username');
  var passInput = document.getElementById('password');
  var errMsg    = document.getElementById('errMsg');
  var errText   = document.getElementById('errText');
  var submitBtn = document.getElementById('submitBtn');
  var togglePw  = document.getElementById('togglePw');
  var eyeIcon   = document.getElementById('eyeIcon');

  // ── Toggle password visibility ─────────────────────────────────────────
  var eyeOpen   = '<path d="M1 12S5 4 12 4s11 8 11 8-4 8-11 8S1 12 1 12z"/><circle cx="12" cy="12" r="3"/>';
  var eyeClosed = '<path d="M17.94 17.94A10.1 10.1 0 0 1 12 20C5 20 1 12 1 12a18.1 18.1 0 0 1 5.06-5.94"/>' +
                  '<path d="M9.9 4.24A9.1 9.1 0 0 1 12 4c7 0 11 8 11 8a18.5 18.5 0 0 1-2.16 3.19"/>' +
                  '<line x1="1" y1="1" x2="23" y2="23"/>';

  togglePw.addEventListener('click', function() {
    var show = passInput.type === 'password';
    passInput.type = show ? 'text' : 'password';
    eyeIcon.innerHTML = show ? eyeClosed : eyeOpen;
    passInput.focus();
  });

  // ── Hide error on input ────────────────────────────────────────────────
  [userInput, passInput].forEach(function(el) {
    el.addEventListener('input', function() { errMsg.classList.remove('show'); });
  });

  // ── Form submission ────────────────────────────────────────────────────
  form.addEventListener('submit', function(e) {
    e.preventDefault();

    var user = userInput.value.trim();
    var pass = passInput.value;

    if (!user || !pass) {
      showError('All fields are required');
      return;
    }

    setLoading(true);

    // haserl requires x-www-form-urlencoded to populate POST_* vars
    fetch('/cgi-bin/auth.cgi', {
      method: 'POST',
      headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
      body: new URLSearchParams({ username: user, password: pass })
    })
    .then(function(resp) {
      if (resp.ok) {
        return resp.json().then(function(data) {
          if (data.ok) {
            // Success — redirect to protected page
            window.location.href = data.redirect || '/cgi-bin/index.cgi';
          } else {
            showError(data.error || 'Invalid username or password');
          }
        });
      } else if (resp.status === 401) {
        showError('Invalid username or password');
      } else if (resp.status === 429) {
        showError('Too many attempts. Try again later');
      } else {
        showError('Server error: ' + resp.status);
      }
    })
    .catch(function() {
      showError('Cannot connect to server');
    })
    .then(function() { setLoading(false); }, function() { setLoading(false); }); // ES5 .finally() shim
  });

  function showError(msg) {
    errText.textContent = msg;
    errMsg.classList.remove('show');
    void errMsg.offsetWidth; // reflow to replay animation
    errMsg.classList.add('show');
  }

  function setLoading(on) {
    submitBtn.classList.toggle('loading', on);
    submitBtn.disabled = on;
  }

  userInput.focus();

})();
