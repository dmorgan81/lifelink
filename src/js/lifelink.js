/* global Pebble */
/* global html */

/* Base64.js v0.3.0 */
!function(){function t(t){this.message=t}var r="undefined"!=typeof exports?exports:this,e="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";t.prototype=new Error,t.prototype.name="InvalidCharacterError",r.btoa||(r.btoa=function(r){for(var o,n,a=String(r),i=0,c=e,d="";a.charAt(0|i)||(c="=",i%1);d+=c.charAt(63&o>>8-i%1*8)){if(n=a.charCodeAt(i+=.75),n>255)throw new t("'btoa' failed: The string to be encoded contains characters outside of the Latin1 range.");o=o<<8|n}return d}),r.atob||(r.atob=function(r){var o=String(r).replace(/=+$/,"");if(o.length%4==1)throw new t("'atob' failed: The string to be decoded is not correctly encoded.");for(var n,a,i=0,c=0,d="";a=o.charAt(c++);~a&&(n=i%4?64*n+a:a,i++%4)?d+=String.fromCharCode(255&n>>(-2*i&6)):0)a=e.indexOf(a);return d})}();

function btos(b) { return b ? 'on' : 'off'; }
function stob(s) { return s === 'on' ? true : false; }

Pebble.addEventListener("ready", function(e) {
    html = atob(html);
});

Pebble.addEventListener("showConfiguration", function (e) {
    var config = JSON.parse(localStorage.getItem('config') || '{}');
    config['round-enabled'] = btos(config['round-enabled']);
    var a = html.replace('REPLACE_ME', JSON.stringify(config));
    Pebble.openURL("data:html;base64," + btoa(a));
});

Pebble.addEventListener("webviewclosed", function(e) {
    if (e.response && e.response.length) {
        console.log(e.response);
        var o = JSON.parse(e.response);
        o['round-enabled'] = stob(o['round-enabled']);
        localStorage.setItem('config', JSON.stringify(o));
        Pebble.sendAppMessage(o);
    }
});