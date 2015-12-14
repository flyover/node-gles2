/**
 * Copyright (c) Flyover Games, LLC.  All rights reserved. 
 *  
 * Permission is hereby granted, free of charge, to any person 
 * obtaining a copy of this software and associated 
 * documentation files (the "Software"), to deal in the Software 
 * without restriction, including without limitation the rights 
 * to use, copy, modify, merge, publish, distribute, sublicense, 
 * and/or sell copies of the Software, and to permit persons to 
 * whom the Software is furnished to do so, subject to the 
 * following conditions: 
 *  
 * The above copyright notice and this permission notice shall 
 * be included in all copies or substantial portions of the 
 * Software. 
 *  
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY 
 * KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE 
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
 * PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR 
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR 
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 
 */

var node_gles2 = null;
try { node_gles2 = node_gles2 || require('./build/Release/node-gles2.node'); } catch (err) {}
try { node_gles2 = node_gles2 || process._linkedBinding('node_gles2'); } catch (err) {}
try { node_gles2 = node_gles2 || process.binding('node_gles2'); } catch (err) {}
module.exports = node_gles2;

/// var node_gles2 = require('@flyover/node-gles2');
/// var gl = node_gles2.gl();
/// node_gles2.gl* -> gl.*
/// node_gles2.GL_* -> gl.*
node_gles2.gl = function (out) {
	out = out || {};
	var re = /^(gl|GL_)(.*)/;
	for (var key in node_gles2) {
		var match = key.match(re);
		if (match && match[2]) {
			//console.log(key, match[2]);
			out[match[2]] = node_gles2[key];
		} else {
			//console.log("!!!", key);
			out[key] = node_gles2[key];
		}
	}
	return out;
}

//node_gles2.gl();
