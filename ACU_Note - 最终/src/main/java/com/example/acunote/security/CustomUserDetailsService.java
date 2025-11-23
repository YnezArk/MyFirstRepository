package com.example.acunote.security;

import com.example.acunote.model.User;
import com.example.acunote.repository.UserRepository;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.security.core.userdetails.UserDetails;
import org.springframework.security.core.userdetails.UserDetailsService;
import org.springframework.security.core.userdetails.UsernameNotFoundException;
import org.springframework.stereotype.Service;
import java.util.Collections;

@Service
public class CustomUserDetailsService implements UserDetailsService {
// 用户详情服务
    @Autowired
    private UserRepository userRepository;
// 根据用户名加载用户详情
    @Override
    public UserDetails loadUserByUsername(String username) {
        User user = userRepository.findByUsername(username)
                .orElseThrow(() -> new UsernameNotFoundException("User not found: " + username));
// 创建Spring Security的用户详情对象
        return new org.springframework.security.core.userdetails.User(
                user.getUsername(),
                user.getPassword(), // 未加密，仅演示
                true,  // enabled
                true,  // accountNonExpired
                true,  // credentialsNonExpired
                true,  // accountNonLocked
                Collections.emptyList() //
        );
    }
}