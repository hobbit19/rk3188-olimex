jQuery(
	function($) {
		$('.dropDown' ).each( 
			function() {
				var $this = $(this);
				$this.
					mouseenter(
						function () {
							$this.children().show();
						}
					).
					mouseleave(
						function () {
							$this.children().hide();
						}
					)
				;
			}
		);
	}
);
