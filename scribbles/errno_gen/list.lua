return function(ctx)
	local neg = ctx.econd("minus_one", "(e == -1)")

	ctx.define(
		-- normal sync write where we don't care about EAGAIN or such.
		-- other wrappers would be defined later for those cases.
		-- note second arg == nil = wrapper name same as the called libc function.
		-- fourth value being false means there are no special-case errno values we expect.
		-- (e.g. for sync writes we expect EINTR to be hidden from us)
		{ "write", nil, neg, false }
	)
end

