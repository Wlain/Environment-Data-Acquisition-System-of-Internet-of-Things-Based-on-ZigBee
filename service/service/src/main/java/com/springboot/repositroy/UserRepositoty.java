package com.springboot.repositroy;

import com.springboot.entities.User;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.data.jpa.repository.Query;
import org.springframework.data.repository.query.Param;
import org.springframework.stereotype.Repository;


/**
 * Created by 默默 on 2017/5/6.
 */
@Repository
public interface UserRepositoty extends JpaRepository<User,Long>{
    @Query("select t from User t where t.last_name = :last_name")
    User findByUserName(@Param("last_name") String name);
}
