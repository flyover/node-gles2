{
	'targets':
	[
		{
			'target_name': "node-gles2",
			'include_dirs':
			[   
				"<(module_root_dir)",
				"<!(node -e \"require('nan')\")"
			],
			'sources':
			[
				"node-gles2.cc"
			],
			'conditions':
			[
				[
					'OS=="linux"',
					{
						'libraries': []
					},
				],
				[
					'OS=="mac"',
					{
						'sources': [ "mac/bridge-gl-mac.cc" ],
						'include_dirs': [ "mac" ],
						'libraries': [ "-framework OpenGL" ]
					},
				],
				[
					'OS=="win"',
					{
						'sources': [ "win/bridge-gl-win.cc" ],
						'include_dirs': [ "win" ],
						'libraries': [ "opengl32.lib" ]
					},
				]
			]
		}
	]
}

